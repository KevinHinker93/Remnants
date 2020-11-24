// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShootergameCharacter.h"
#include "Core/Items/ShootergameProjectile.h"
#include "Core/Items/AModule.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/ActorStatsComponent.h"
#include "Components/ShieldComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Components/CharacterStateComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/CameraControllerComponent.h"
#include "Components/CapsuleCollControllerComponent.h"
#include "Components/CharacterInteractionComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Components/ElementEffectComponent.h"
#include "ElementStatusEffects/ElementStatusEffect.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "EventSystemHandlerSingleton.h"
#include "../Items/AElement.h"
#include "../Items/AKeyItem.h"
#include "../../ElementDamageType.h"
#include "../../Components/HitComboComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AShootergameCharacter

AShootergameCharacter::AShootergameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AShootergameCharacter::OnOverlapBegin);
	//GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AShootergameCharacter::OnOverlapEnd);
	//OnActorEndOverlap.AddDynamic(this, &AShootergameCharacter::ActorEndOverlap);

	// Create a CameraComponent	
	m_firstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	m_firstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	m_firstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	m_firstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	m_mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	m_mesh1P->SetOnlyOwnerSee(true);
	m_mesh1P->SetupAttachment(m_firstPersonCameraComponent);
	m_mesh1P->bCastDynamicShadow = false;
	m_mesh1P->CastShadow = false;
	m_mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	m_mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	m_fpGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	m_fpGun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	m_fpGun->bCastDynamicShadow = false;
	m_fpGun->CastShadow = false;
	m_fpGun->SetupAttachment(RootComponent);

	m_muzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	m_muzzleLocation->SetupAttachment(m_firstPersonCameraComponent);
	m_muzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	m_weaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	m_weaponComponent->SetupAttachment(m_firstPersonCameraComponent);
	m_inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	m_actorStatsComponent = CreateDefaultSubobject<UActorStatsComponent>(TEXT("Stats"));
	m_shieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield"));
	m_healthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	m_hitComboComponent = CreateDefaultSubobject<UHitComboComponent>(TEXT("Combo"));
	m_elementEffectComponent = CreateDefaultSubobject<UElementEffectComponent>(TEXT("ElementEffect"));
	m_stateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("State"));
	m_staminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
	m_cameraControllerComponent = CreateDefaultSubobject<UCameraControllerComponent>(TEXT("Camera Controll"));
	m_capsuleControllerComponent = CreateDefaultSubobject<UCapsuleCollControllerComponent>(TEXT("Capsule Controll"));
	m_interactionComponent = CreateDefaultSubobject<UCharacterInteractionComponent>(TEXT("Character interaction"));

	//m_interactionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction collision"));


	//m_interactionCollision->SetupAttachment(RootComponent);

	// Default offset from the character location for projectiles to spawn
	m_gunOffset = FVector(100.0f, 0.0f, 10.0f);

	// set our turn rates for input
	m_turnDirection = 1.0f;
	m_baseTurnMult = 1.0f;
	m_baseTurnRate = 45.f;
	m_lookUpDirection = 1.0f;
	m_baseLookUpMult = 1.0f;
	m_baseLookUpRate = 45.f;
	m_moveSpeedMult = 1.0f;

	/*m_baseRunSpeed = 1.0f;
	m_SprintSpeedMult = 1.5f;
	m_CrouchSpeedMult = 0.8f;
	m_characterStandingHeight = 96.0f;
	m_characterCrouchHeightDiff = 25.0f;
	m_characterArmHeightDiff = 32.0f;*/
	m_damageResistance = 1.0f;
}

USceneComponent* AShootergameCharacter::GetMuzzelLoc()
{
	UE_LOG(LogTemp, Display, TEXT("Fetch muzzle from player"));
	return m_muzzleLocation;
}

void AShootergameCharacter::WeaponFireAnimation()
{
	if (m_fireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = m_mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(m_fireAnimation, 1.f);
		}
	}
}

float AShootergameCharacter::GetCurrentOverheatPercentage() const
{
	return m_weaponComponent->GetCurrentOverheatedPercentage();
}

float AShootergameCharacter::GetDamageMultiplier() const
{
	return m_hitComboComponent->GetCurrentDamageMultiplier();
}

void AShootergameCharacter::RemoveStatusEffect(ElementEffectType elementStatusEffectType)
{
	m_elementEffectComponent->RemoveElmentStatusEffect(elementStatusEffectType);
}

int AShootergameCharacter::GetVictimMaxHealth()
{
	m_criticalSection.Lock();
	int victimMaxHealth = m_healthComponent->GetMaxHealth();
	m_criticalSection.Unlock();
	return victimMaxHealth;
}

int AShootergameCharacter::GetVictimMaxElementImpactVal()
{
	return m_elementEffectComponent->GetElementStatusEffectResistanceVal();
}

float AShootergameCharacter::GetVictimElementStatusEffectDepleteAmount()
{
	return m_elementEffectComponent->GetElementStatusEffectDepleteAmount();
}

float AShootergameCharacter::GetVictimElementStatusEffectDepleteTime()
{
	return m_elementEffectComponent->GetElementStatusEffectDepleteTime();
}

class UShieldComponent* AShootergameCharacter::GetVictimShieldComponent()
{
	return m_shieldComponent;
}

void AShootergameCharacter::SetFloatVal(EntityStats stat, float val)
{
	if (stat == EntityStats::FiringSpeed)
	{
		float newVal = m_weaponComponent->GetFiringSpeedMult() * val;
		m_weaponComponent->SetFiringSpeedMult(newVal);
	}
	else if (stat == EntityStats::MovementSpeed)
	{
		m_moveSpeedMult += val - 1;
	}
	//else if (stat == EntityStats::DamageResistance)
	//{
	//	float newVal = m_shieldComponent->GetReceivedDamageMult() * val;
	//	m_shieldComponent->SetReceivedDamageMult(newVal);
	//}
	else if (stat == EntityStats::HealthRegen)
	{
		if (val >= 1.0f)
			m_healthComponent->SetHealthRegeneratable(true);
		else
			m_healthComponent->SetHealthRegeneratable(false);
	}
	else if (stat == EntityStats::DamageResistance)
	{
		m_damageResistance *= val;
	}
	else if (stat == EntityStats::MaxShield)
	{
		int newVal = m_shieldComponent->GetMaxShield() * val;
		m_shieldComponent->SetMaxShield(newVal);
	}
	else if (stat == EntityStats::OverheatingFactor)
	{
		float newVal = m_weaponComponent->GetOverheatingMult() * val;
		m_weaponComponent->SetOverheatingMult(newVal);
	}
	else if (stat == EntityStats::Damage)
	{
		float newVal = m_weaponComponent->GetDamageMult() * val;
		m_weaponComponent->SetDamageMult(newVal);
	}
	else if (stat == EntityStats::CurrentShield)
	{
		float Percentage = FMath::RandRange(0, 1);
		if (Percentage <= val)
		{
			UE_LOG(LogTemp, Warning, TEXT("Set Shield to 0"));
			m_shieldComponent->SetShield(0);
		}
	}
	else if (stat == EntityStats::MaxElementEffectResistance)
	{
		int newVal = m_elementEffectComponent->GetElementStatusEffectResistanceVal() * val;
		m_elementEffectComponent->SetElementStatusEffectResistanceVal(newVal);
	}
	else if (stat == EntityStats::ElementEffectImpactFac)
	{
		float newVal = m_elementEffectComponent->GetElementStatusEffectImpactFac() * val;
		m_elementEffectComponent->SetElementStatusEffectImpactFac(newVal);
	}
}

void AShootergameCharacter::ResetFloatVal(EntityStats stat, float val)
{
	if (stat == EntityStats::FiringSpeed)
	{
		float newVal = m_weaponComponent->GetFiringSpeedMult() / val;
		m_weaponComponent->SetFiringSpeedMult(newVal);
	}
	else if (stat == EntityStats::MovementSpeed)
	{
		m_moveSpeedMult += 1 - val;
	}
	else if (stat == EntityStats::DamageResistance)
	{
		float newVal = m_shieldComponent->GetReceivedDamageMult() / val;
		m_shieldComponent->SetReceivedDamageMult(newVal);
	}
	else if (stat == EntityStats::MaxShield)
	{
		int newVal = m_shieldComponent->GetMaxShield() / val;
		if (fmod(m_shieldComponent->GetMaxShield(), val) != 0)
			newVal++;

		m_shieldComponent->SetMaxShield(newVal);
	}
	else if (stat == EntityStats::OverheatingFactor)
	{
		float newVal = m_weaponComponent->GetOverheatingMult() / val;
		m_weaponComponent->SetOverheatingMult(newVal);
	}
	else if (stat == EntityStats::Damage)
	{
		float newVal = m_weaponComponent->GetDamageMult() / val;
		m_weaponComponent->SetDamageMult(newVal);
	}
	else if (stat == EntityStats::CurrentShield)
	{

	}
	else if (stat == EntityStats::MaxElementEffectResistance)
	{
		int newVal = m_elementEffectComponent->GetElementStatusEffectResistanceVal() / val;
		if (fmod(m_elementEffectComponent->GetElementStatusEffectResistanceVal(), val) != 0)
			newVal++;
		m_elementEffectComponent->SetElementStatusEffectResistanceVal(newVal);
	}
	else if (stat == EntityStats::ElementEffectImpactFac)
	{
		float newVal = m_elementEffectComponent->GetElementStatusEffectImpactFac() / val;
		m_elementEffectComponent->SetElementStatusEffectImpactFac(newVal);
	}
}

void AShootergameCharacter::ChangeState(EntityState state)
{
	if (state == EntityState::Paralysed)
		m_stateComponent->SetCurrentState(CharacterStates::Paralysed);
	else if (state == EntityState::Frozen)
		m_weaponComponent->SetWeaponState(WeaponStates::Frozen);
}

void AShootergameCharacter::ResetState(EntityState state)
{
	if (state == EntityState::Paralysed)
		m_stateComponent->SetCurrentState(m_stateComponent->GetPreviousState()->GetStateType());
	else if (state == EntityState::Frozen)
		m_weaponComponent->ResetWeaponState();
}

float AShootergameCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float actualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser) * m_shieldComponent->GetReceivedDamageMult();

	UElementDamageType *damageType = Cast<UElementDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());

	EDamageEffectiveness damageEffectiveness = EDamageEffectiveness::Normal;

	if (damageType)
	{
		int randSeed = FMath::Rand();
		FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
		float randomMult = FMath::RandRange(damageType->GetRandomDamageMultRange().m_randomDamageMultMin, damageType->GetRandomDamageMultRange().m_randomDamageMultMax);
		actualDamage *= randomMult;
		actualDamage *= m_damageResistance;
	}
	else
	{
		int randSeed = FMath::Rand();
		FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
		float randomMult = FMath::RandRange(0.9f, 1.1f);
		actualDamage *= randomMult;
		actualDamage *= m_damageResistance;

		if (m_shieldComponent)
			m_shieldComponent->UpdateShield(-actualDamage);
		else
			UE_LOG(LogTemp, Warning, TEXT("No component to damage, when hitting %s: "), *GetName());
	}

	if (damageType)
	{
		FName elementName = damageType->GetElementTypeName();
		ElementEffectType elementType = damageType->GetElementType();

		if (elementType == ElementEffectType::Crystal)
		{
			if (m_shieldComponent->GetShield() > 0)
			{
				if (m_healthComponent)
				{
					actualDamage *= damageType->GetElementStatusEffectStrenghts()[0];
					m_healthComponent->UpdateHealth(-actualDamage);
				}
			}
			else
				m_healthComponent->UpdateHealth(-actualDamage);

			//return Super::TakeDamage(actualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			if (m_shieldComponent)
				m_shieldComponent->UpdateShield(-actualDamage);
			else
				UE_LOG(LogTemp, Warning, TEXT("No component to damage, when hitting %s: "), *GetName());
		}

		bool check = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
		eventHandler->OnReceiveDamage.Broadcast(m_shieldComponent, -actualDamage, damageEffectiveness);
		m_hitComboComponent->WasDamaged(actualDamage);

		/*if (elementType == ElementEffectType::Plasma)
		{
		m_shieldComponent->UpdateShield(-actualDamage);
		return Super::TakeDamage(actualDamage, DamageEvent, EventInstigator, DamageCauser);
		}*/
	}
		// Removed for now ------------------------------------
	//	float impact = damageType->GetElementStatusEffectImpactMult();
	//	float baseImpact = damageType->GetElementStatusEffectBaseImpact();
	//	float wantImpact = ((impact * actualDamage) + baseImpact) * m_elementEffectComponent->GetElementStatusEffectImpactFac();

	//	IElementStatusEffectVictim* victim = Cast<IElementStatusEffectVictim>(this);
	//	if (!m_elementEffectComponent->ElementStatusEffectExists(elementType))
	//	{
	//		if (victim)
	//		{
	//			//Set Spawn Collision Handling Override
	//			FActorSpawnParameters ActorSpawnParams;
	//			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//			ActorSpawnParams.OverrideLevel = GetLevel();
	//			AElementStatusEffect* statusEffect = GetWorld()->SpawnActor<AElementStatusEffect>(damageType->GetStatusEffectClass(), GetActorLocation(), GetControlRotation(), ActorSpawnParams);
	//			statusEffect->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

	//			m_elementEffectComponent->AddElementStatusEffect(elementType, statusEffect);

	//			statusEffect->InitElementEffect(GetWorld(), elementType, damageType->GetElementStatusEffectStrenghts(), damageType->GetElementStatusEffectInterval(), victim, wantImpact);
	//		}
	//		else
	//			UE_LOG(LogDamage, Error, TEXT("%s: cannot receive statusEffect"), *GetName()); // Happens when entity has no elementVictim interface
	//	}
	//	else if (!m_elementEffectComponent->ElementStatusEffectIsActivated(elementType))
	//	{
	//		UE_LOG(LogDamage, Verbose, TEXT("Status effect already exists, just adding value"));
	//		m_elementEffectComponent->AddImpactToElementStatusEffect(elementType, wantImpact);
	//	}
	//	UE_LOG(LogDamage, VeryVerbose, TEXT("Damage %s: %f ---Element: %s --- Impact: %f"), *GetName(), actualDamage, *elementName.ToString(), wantImpact);

	//	bool wantsToBeActivated = m_elementEffectComponent->ElementStatusEffectWantsToBeActivated(elementType);
	//	if (wantsToBeActivated)
	//		m_elementEffectComponent->ActivateElementStatusEffect(elementType);
	//}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("DamageType is null when hitting %s: "), *GetName());

	// --------------------------------------


	return Super::TakeDamage(actualDamage, DamageEvent, EventInstigator, DamageCauser);
}

void AShootergameCharacter::UpdateStats()
{
	m_healthComponent->UpdateStats();
	m_shieldComponent->UpdateStats();
	m_weaponComponent->UpdateStats();
	m_elementEffectComponent->UpdateStats(true);
}

void AShootergameCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_cameraControllerComponent->SetCameraToControll(m_firstPersonCameraComponent);
	m_capsuleControllerComponent->SetCapsuleToControll(GetCapsuleComponent());

	m_fpGun->AttachToComponent(m_mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	/*for (int i = 0; i < m_defaultModules.Num(); i++)
	{
		AAWeaponModule* module = m_defaultModules[i].GetDefaultObject();
		AAWeaponModule* newModule = DuplicateObject(module, module->GetOuter());
		m_weaponComponent->SetModuleAtSlotIdx(i, newModule);
	}*/

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	//eventHandler->OnPickup.AddDynamic(GetComponentByClass(UInventoryComponent::StaticClass), &AShootergameCharacter::PickupItem);
	eventHandler->OnCharacterStopSprinting.AddDynamic(this, &AShootergameCharacter::OnStopSprinting);
	eventHandler->OnEnemyDied.AddDynamic(this, &AShootergameCharacter::OnEnemyDied);

	eventHandler->OnExternalChangeState.AddDynamic(this, &AShootergameCharacter::OnExternalChangeState);
	eventHandler->OnExternalResetState.AddDynamic(this, &AShootergameCharacter::OnExternalResetState);

	// Assign stats component, to enable the other components to fetch their stats
	m_healthComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_healthComponent->SetHealthToMax();
	m_shieldComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_shieldComponent->SetShieldToMax();
	m_weaponComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_elementEffectComponent->SetActorStatsComponent(m_actorStatsComponent);
	
	// Set Player stats
	UpdateStats();
}

void AShootergameCharacter::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	//eventHandler->OnPickup.RemoveDynamic(this, &AShootergameCharacter::PickupItem);
	eventHandler->OnCharacterStopSprinting.RemoveDynamic(this, &AShootergameCharacter::OnStopSprinting);
	eventHandler->OnEnemyDied.RemoveDynamic(this, &AShootergameCharacter::OnEnemyDied);
	eventHandler->OnExternalChangeState.RemoveDynamic(this, &AShootergameCharacter::OnExternalChangeState);
	eventHandler->OnExternalResetState.RemoveDynamic(this, &AShootergameCharacter::OnExternalResetState);
}

void AShootergameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); // Call parent class tick function  
}

void AShootergameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShootergameCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AShootergameCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShootergameCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("FirePrimary", IE_Pressed, this, &AShootergameCharacter::OnFirePrimaryButtonPressed);
	PlayerInputComponent->BindAction("FirePrimary", IE_Released, this, &AShootergameCharacter::OnFirePrimaryButtonReleased);

	PlayerInputComponent->BindAction("FireSecondary", IE_Pressed, this, &AShootergameCharacter::OnFireSecondaryButtonPressed);
	PlayerInputComponent->BindAction("FireSecondary", IE_Released, this, &AShootergameCharacter::OnFireSecondaryButtonReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShootergameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Strafe", this, &AShootergameCharacter::Strafe);

	PlayerInputComponent->BindAction("ChangeElementSetPos",IE_Pressed, this, &AShootergameCharacter::ChangeElementPos);
	PlayerInputComponent->BindAction("ChangeElementSetNeg", IE_Pressed, this, &AShootergameCharacter::ChangeElementNeg);

	PlayerInputComponent->BindAction("CharacterInteract", IE_Pressed, this, &AShootergameCharacter::Interact);
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AShootergameCharacter::TurnWMouse);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShootergameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AShootergameCharacter::LookUpWMouse);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShootergameCharacter::LookUpAtRate);
}

void AShootergameCharacter::OnFirePrimaryButtonPressed()
{
	if (m_isDead)
		return;

	m_weaponComponent->BeginShooting(0);
	UE_LOG(LogTemp, Verbose, TEXT("Actor: %s current Health: %d"), *GetName(), m_healthComponent->GetMaxHealth());
	UE_LOG(LogTemp, Verbose, TEXT("Actor: %s current Shield: %d"), *GetName(), m_shieldComponent->GetMaxShield());
}

void AShootergameCharacter::OnFirePrimaryButtonReleased()
{
	m_weaponComponent->StopFiring(0);
}

void AShootergameCharacter::OnFireSecondaryButtonPressed()
{
	if (m_isDead)
		return;

	m_weaponComponent->BeginShooting(1);
	UE_LOG(LogTemp, Verbose, TEXT("Actor: %s current Health: %d"), *GetName(), m_healthComponent->GetMaxHealth());
	UE_LOG(LogTemp, Verbose, TEXT("Actor: %s current Shield: %d"), *GetName(), m_shieldComponent->GetMaxShield());
}

void AShootergameCharacter::OnFireSecondaryButtonReleased()
{
	m_weaponComponent->StopFiring(1);
}

void AShootergameCharacter::ToggleCrouch()
{
	if (m_isDead)
		return;

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		if (m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Crouching)
			m_stateComponent->SetCurrentState(m_stateComponent->GetPreviousState()->GetStateType());
		else
			m_stateComponent->SetCurrentState(CharacterStates::Crouching);
	}
}

void AShootergameCharacter::ChangeElementPos()
{
	if (m_isDead)
		return;

	m_weaponComponent->ChangeElementSet(1.0);
}

void AShootergameCharacter::ChangeElementNeg()
{
	if (m_isDead)
		return;

	m_weaponComponent->ChangeElementSet(-1.0f);
}

void AShootergameCharacter::MoveForward(float Value)
{
	if (m_isDead)
		return;

	m_stateComponent->GetCurrentState()->MoveForward(Cast<APawn>(this), Value, m_moveSpeedMult);
}

void AShootergameCharacter::Strafe(float Value)
{
	if (m_isDead)
		return;

	m_stateComponent->GetCurrentState()->Strafe(Cast<APawn>(this), Value, m_moveSpeedMult);
}

void AShootergameCharacter::TurnWMouse(float Rate)
{
	if (m_isDead)
		return;

	AddControllerYawInput(Rate * m_baseTurnMult  * m_turnDirection);
}

void AShootergameCharacter::TurnAtRate(float Rate)
{
	if (m_isDead)
		return;

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * m_baseTurnRate * GetWorld()->GetDeltaSeconds() * m_turnDirection);
}

void AShootergameCharacter::LookUpWMouse(float Rate)
{
	if (m_isDead)
		return;

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * m_baseLookUpMult * m_lookUpDirection);
}

void AShootergameCharacter::LookUpAtRate(float Rate)
{
	if (m_isDead)
		return;

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * m_baseLookUpRate * GetWorld()->GetDeltaSeconds() * m_lookUpDirection);
}

void AShootergameCharacter::Interact()
{
	if (m_isDead)
		return;

	if (m_interactionComponent)
		m_interactionComponent->Interact();
}

void AShootergameCharacter::OnStopSprinting(APawn* sprintingPawn)
{
	if (m_isDead)
		return;

	if (sprintingPawn == Cast<APawn>(this))
	{
		//if (m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Sprinting)
		//	m_stateComponent->SetCurrentState(m_stateComponent->GetPreviousState()->GetStateType());
		//else
		if (m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Sprinting)
			m_stateComponent->SetCurrentState(CharacterStates::Running);
		else if(m_stateComponent->GetPreviousState()->GetStateType() != CharacterStates::HeavyWeapon)
			m_stateComponent->SetPreviousState(CharacterStates::Running);
	}
}

void AShootergameCharacter::OnEnemyDied(float exp, FName tagOfDeathCauser)
{
	if (m_isDead)
		return;

	if (tagOfDeathCauser == Tags[0])
		AddExp(exp);
}

void AShootergameCharacter::OnExternalChangeState(AActor* actorToChangeState, FName desiredState)
{
	if (this == actorToChangeState)
	{
		if (desiredState == "HeavyWeapon")
		{
			if (m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Paralysed)
				m_stateComponent->SetPreviousState(CharacterStates::HeavyWeapon);
			else
				m_stateComponent->SetCurrentState(CharacterStates::HeavyWeapon);
		}
	}
}

void AShootergameCharacter::OnExternalResetState(AActor* actorToChangeState)
{
	if (this == actorToChangeState && m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::HeavyWeapon)
		m_stateComponent->ResetCurrentState();
	else if(this == actorToChangeState && m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Crouching)
		m_stateComponent->SetPreviousState(CharacterStates::Running);
}

void AShootergameCharacter::ChangeLevel(int level)
{
	m_actorStatsComponent->SetLevel(level);
	UpdateStats();
}

void AShootergameCharacter::AddExp(float exp)
{
	m_currentExp += exp;
	UE_LOG(LogTemp, Display, TEXT("Player:: %s Added exp: %f // %f // %f"), *GetName(), exp, m_currentExp, m_neededExpForLvlUp);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FString notificationType = "exp";
	eventHandler->OnReceiveMiscNotification.Broadcast(notificationType, int(exp));

	CheckForLvlUp();
}

void AShootergameCharacter::CheckForLvlUp()
{
	// How often player has lvldUp
	int lvlUpAmount = 0;
	while (m_currentExp >= m_neededExpForLvlUp && m_neededExpForLvlUp > 0)
	{
		lvlUpAmount++;
		
		float expOverlap = FMath::Abs(m_neededExpForLvlUp - m_currentExp);
		m_currentExp = 0 + expOverlap;
		m_neededExpForLvlUp += m_neededExpForLvlUp * m_neededExpPerLevelMult;
	}

	// Display ui message that player lvld up
	if(lvlUpAmount > 0)
	{
		ChangeLevel(m_actorStatsComponent->GetLevel() + lvlUpAmount);
		UE_LOG(LogTemp, Display, TEXT("Player LVL UP: %d"), m_actorStatsComponent->GetLevel());
		m_healthComponent->SetHealthToMax();
		m_shieldComponent->SetShieldToMax();
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		FString levelUpDisplayText = "You reached lvl ";
		levelUpDisplayText.Append(FString::FromInt( m_actorStatsComponent->GetLevel()));
		eventHandler->OnDisplayUIMessage.Broadcast(*levelUpDisplayText, -1);
		eventHandler->OnPlayerLvlUp.Broadcast(m_actorStatsComponent->GetLevel());
		LvlUp();
	}
}

void AShootergameCharacter::StartSprinting()
{
	if (m_isDead)
		return;

	if(!m_staminaComponent->IsExhausted() && m_stateComponent->GetCurrentState()->GetStateType() != CharacterStates::HeavyWeapon
		&& m_stateComponent->GetPreviousState()->GetStateType() != CharacterStates::HeavyWeapon)
		m_stateComponent->SetCurrentState(CharacterStates::Sprinting);
	else
		UE_LOG(LogTemp, Display, TEXT("Is Exhausted"));
}

void AShootergameCharacter::StopSprinting()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (checkSingleton)
	{
		eventHandler->OnCharacterStopSprinting.Broadcast(Cast<APawn>(this));
	}
}

void AShootergameCharacter::Kill()
{
	if (m_isDead)
		return;

	UE_LOG(LogTemp, Display, TEXT("Player was killed"));
	m_isDead = true;
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	// First notify the achievement manager that player dies
	eventHandler->OnPlayerDiedAchievementNotif.Broadcast();
	// Then tell level manager to switch to death screen
	eventHandler->OnPlayerDied.Broadcast();
	m_hitComboComponent->EndCombo();
}

void AShootergameCharacter::Reset()
{
	m_healthComponent->SetHealthToMax();
	m_shieldComponent->SetShieldToMax();
	m_weaponComponent->ResetWeaponState();
	m_weaponComponent->ResetOverheat();
	for (int i = 0; i < m_weaponComponent->GetModuleCount(); i++)
		m_weaponComponent->StopFiring(i);
	m_elementEffectComponent->RemoveAllElementStatusEffects();
	m_isDead = false;
	m_hitComboComponent->EndCombo();
}