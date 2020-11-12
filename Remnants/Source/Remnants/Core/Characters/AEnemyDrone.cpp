// Fill out your copyright notice in the Description page of Project Settings.

#include "AEnemyDrone.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ActorStatsComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ShieldComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/ElementEffectComponent.h"
#include "Components/CharacterStateComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "ElementStatusEffects/ElementStatusEffect.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Core/Items/AModule.h"
#include "Core/Items/AElement.h"
#include "Core/Items/MachineGunModule.h"
#include "ElementDamageType.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "EventSystemHandlerSingleton.h"
#include "Core/Items/AItem.h"

AAEnemyDrone::AAEnemyDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	m_collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	m_collisionComponent->InitSphereRadius(5.0f);
	m_collisionComponent->SetupAttachment(RootComponent);

	m_skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Enemy Mesh"));
	m_skeletalMesh->SetupAttachment(RootComponent);

	m_actorStatsComponent = CreateDefaultSubobject<UActorStatsComponent>(TEXT("Stats"));
	m_healthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	m_shieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield"));
	m_weaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	m_elementEffectComponent = CreateDefaultSubobject<UElementEffectComponent>(TEXT("ElementEffect"));
	m_stateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("States"));
	m_widgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	m_muzzleLoc = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	m_muzzleLoc->SetupAttachment(RootComponent);

	m_weaponComponent->SetupAttachment(RootComponent);

	m_canMove = true;
	m_canShoot = true;
	m_moveSpeedMult = 1.0f;
	m_pushPullResistance = 40.0f;
	m_droppedExp = 200;
	m_droppedExpBase = 200;
	m_droppedExpLevelMult = 0.15f;

	m_lastHittedElement = ElementEffectType::Fire;
}

void AAEnemyDrone::WeaponFireAnimation()
{
}

USceneComponent* AAEnemyDrone::GetMuzzelLoc()
{
	return m_muzzleLoc;
}

float AAEnemyDrone::GetCurrentOverheatPercentage() const
{
	return m_weaponComponent->GetCurrentOverheatedPercentage();
}

void AAEnemyDrone::RemoveStatusEffect(ElementEffectType elementStatusEffectType)
{
	m_elementEffectComponent->RemoveElmentStatusEffect(elementStatusEffectType);
}

int AAEnemyDrone::GetVictimMaxHealth()
{
	m_criticalSection.Lock();
	int victimMaxHealth = m_healthComponent->GetMaxHealth();
	m_criticalSection.Unlock();
	return victimMaxHealth;
}

int AAEnemyDrone::GetVictimMaxElementImpactVal()
{
	return m_elementEffectComponent->GetElementStatusEffectResistanceVal();
}

float AAEnemyDrone::GetVictimElementStatusEffectDepleteAmount()
{
	return m_elementEffectComponent->GetElementStatusEffectDepleteAmount();
}

float AAEnemyDrone::GetVictimElementStatusEffectDepleteTime()
{
	return m_elementEffectComponent->GetElementStatusEffectDepleteTime();
}

class UShieldComponent* AAEnemyDrone::GetVictimShieldComponent()
{
	return m_shieldComponent;
}

void AAEnemyDrone::SetFloatVal(EntityStats stat, float val)
{
	if (stat == EntityStats::FiringSpeed)
	{
		float newVal = m_weaponComponent->GetFiringSpeedMult() * val;
		m_weaponComponent->SetFiringSpeedMult(newVal);
	}
	else if (stat == EntityStats::MovementSpeed)
	{
		m_moveSpeedMult += val - 1;
		GetCharacterMovement()->MaxFlySpeed *= m_moveSpeedMult;
	}
	else if (stat == EntityStats::DamageResistance)
	{
		float newVal = m_shieldComponent->GetReceivedDamageMult() * val;
		m_shieldComponent->SetReceivedDamageMult(newVal);
	}
	else if (stat == EntityStats::MaxShield)
	{
		int newVal = m_shieldComponent->GetMaxShield() * val;
		m_shieldComponent->SetMaxShield(newVal);
	}
	else if (stat == EntityStats::MaxHealth)
	{
		int newVal = m_healthComponent->GetMaxHealth() * val;
		m_healthComponent->SetMaxHealth(newVal);
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
	else if (stat == EntityStats::FiringStrat)
	{
		m_newModuleType = val;

		if(!ChangeModuleType(m_newModuleType))
		{
			m_wantChangeModuleType = true;
			m_currentModuleTryCount = 0;
		}
	}
}

void AAEnemyDrone::ResetFloatVal(EntityStats stat, float val)
{
	if (stat == EntityStats::FiringSpeed)
	{
		float newVal = m_weaponComponent->GetFiringSpeedMult() / val;
		m_weaponComponent->SetFiringSpeedMult(newVal);
	}
	else if (stat == EntityStats::MovementSpeed)
	{
		m_moveSpeedMult += 1 - val;
		float newVal = GetCharacterMovement()->MaxFlySpeed / m_moveSpeedMult;
		if (fmod(GetCharacterMovement()->MaxFlySpeed, m_moveSpeedMult) != 0)
			newVal++;
		GetCharacterMovement()->MaxFlySpeed /= newVal;
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

void AAEnemyDrone::ChangeState(EntityState state)
{
	for (size_t i = 0; i < m_Weapons.Num(); i++)
	{
		if (state == EntityState::Paralysed)
			m_stateComponent->SetCurrentState(CharacterStates::Paralysed);
		else if (state == EntityState::Frozen)
			m_Weapons[i]->SetWeaponState(WeaponStates::Frozen);
	}
}

void AAEnemyDrone::ResetState(EntityState state)
{
	for (size_t i = 0; i < m_Weapons.Num(); i++)
	{
		if (state == EntityState::Paralysed)
			m_stateComponent->ResetCurrentState();
		else if (state == EntityState::Frozen)
			m_Weapons[i]->ResetWeaponState();
	}
}

void AAEnemyDrone::Kill()
{
	if (m_isDead)
		return;

	m_isDead = true;

	// notify spawn point that enemy has been killed
	for (auto tag : Super::Tags)
	{
		if (tag.ToString().Contains("Spawn Group: "))
		{
			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
			eventHandler->OnSpawnpointObjectDestroyed.Broadcast(tag.ToString());
			break;
		}
	}

	m_criticalSection.Lock();
	if (IsPendingKill())
		return;

	//m_elementEffectComponent->RemoveAllElementStatusEffects();

	OnDroneKilled();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	int elementCount = m_weaponComponent->GetElementCount();
	int elementIdx = rand() % elementCount;

	// elementSetIdx == 0 because elements are stored in an 1d array, 
	// and elementIdx is automatically between 0 and elementCount
	UAItem* element = m_weaponComponent->GetElement(0, elementIdx);
	auto info = eventHandler->GetEventFacade()->GetItemInfo(element->GetId());

	float disassemble = FMath::FRandRange(0.f, 1.f);
	if (info.m_canBeDisassembled && disassemble <= m_disassembleDroppedItemChance) // disassemble item
	{
		for (auto kvp : info.m_disassembledItems)
		{
			for (int i = 0; i < kvp.m_amount; i++)
			{
				float dropItemChance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				if (dropItemChance <= m_dropItemChance)
					eventHandler->OnPickupSpawnByIdRequest.Broadcast(this, kvp.m_itemId, GetActorTransform(), GetLevel());
			}
		}
	}
	else // drop "whole" item
	{
		float dropItemChance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		if (dropItemChance <= m_dropItemChance)
			eventHandler->OnPickupSpawnRequest.Broadcast(this, element, GetActorTransform(), GetLevel());
	}

	eventHandler->OnEnemyDied.Broadcast(m_droppedExp, "Player");
	eventHandler->OnDroneElementDeathCause.Broadcast(m_lastHittedElement);

	if (m_isKeyHolder)
	{
		eventHandler->OnKeyHolderDied.Broadcast(m_keyItemId);
		//eventHandler->OnPickupSpawnByIdRequest.Broadcast(this, m_keyItemId, GetActorTransform(), GetLevel());
	}

	if (m_destroyOnKill)
		Destroy();
	m_criticalSection.Unlock();
}

float AAEnemyDrone::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		if (m_elementAffinityComp)
		{
			float affinityMult = 1.0f;
			damageEffectiveness = m_elementAffinityComp->GetElementEffectiveness(damageType->GetElementType(), affinityMult);
			actualDamage *= affinityMult;
		}
	}
	else
	{
		int randSeed = FMath::Rand();
		FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
		float randomMult = FMath::RandRange(0.9f, 1.1f);
		actualDamage *= randomMult;
		DamageShield(actualDamage, damageEffectiveness);
		/*if (m_shieldComponent)
			m_shieldComponent->UpdateShield(-actualDamage);
		else
			UE_LOG(LogTemp, Error, TEXT("No component to damage, when hitting enemy %s: "), *GetName());*/
	}

	if (damageType)
	{
		m_tagOfLastDamageCauser = DamageCauser->Tags[0];
		FName elementName = damageType->GetElementTypeName();
		ElementEffectType elementType = damageType->GetElementType();
		m_lastHittedElement = elementType;
		if (elementType == ElementEffectType::Crystal)
		{
			if (m_shieldComponent->GetShield() > 0)
			{
				actualDamage *= damageType->GetElementStatusEffectStrenghts()[0];
				DamageHealth(actualDamage, damageEffectiveness);
			}
			else
			{
				DamageHealth(actualDamage, damageEffectiveness);
			}
		}
		else
		{
			DamageShield(actualDamage, damageEffectiveness);
		}
	}

		// Removed for now -----------------------------
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
	//			UE_LOG(LogDamage, Error, TEXT("Enemy %s: cannot receive statusEffect"), *GetName()); // Happens when enemy has no elementVictim interface
	//	}
	//	else if (!m_elementEffectComponent->ElementStatusEffectIsActivated(elementType))
	//	{
	//		UE_LOG(LogDamage, Display, TEXT("Status effect already exists, just adding value"));
	//		m_elementEffectComponent->AddImpactToElementStatusEffect(elementType, wantImpact);
	//	}
	//	UE_LOG(LogDamage, Display, TEXT("Damage Enemy %s: %f ---Element: %s --- Impact: %f"), *GetName(), actualDamage, *elementName.ToString(), wantImpact);

	//	bool wantsToBeActivated = m_elementEffectComponent->ElementStatusEffectWantsToBeActivated(elementType);
	//	if (wantsToBeActivated)
	//		m_elementEffectComponent->ActivateElementStatusEffect(elementType);
	//}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("DamageType is null, when hitting enemy %s: "), *GetName());

	/*if(m_shieldComponent)
		m_shieldComponent->UpdateShield(-actualDamage);*/
		/*else
			UE_LOG(LogTemp, Error, TEXT("No component to damage, when hitting enemy %s: "), *m_enemyName.ToString());*/

	// -----------------------------------------

	return Super::TakeDamage(actualDamage, DamageEvent, EventInstigator, DamageCauser);
}

// Called when the game starts or when spawned
void AAEnemyDrone::BeginPlay()
{
	Super::BeginPlay();

	m_isDead = false;

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (size_t i = 0; i < Components.Num(); i++)
	{
		UWeaponComponent* weapon = Cast<UWeaponComponent>(Components[i]);
		if (weapon)
		{
			m_Weapons.Add(weapon);
		}
	}

	UElementAffinityComponent* affComp = Cast<UElementAffinityComponent>(GetComponentByClass(UElementAffinityComponent::StaticClass()));

	if (affComp)
		m_elementAffinityComp = affComp;

	m_healthComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_healthComponent->UpdateStats();
	m_healthComponent->SetHealthToMax();

	m_shieldComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_shieldComponent->UpdateStats();
	m_shieldComponent->SetShieldToMax();
	for (size_t i = 0; i < m_Weapons.Num(); i++)
	{
		m_Weapons[i]->SetActorStatsComponent(m_actorStatsComponent);
		m_Weapons[i]->UpdateStats();
	}

	m_elementEffectComponent->SetActorStatsComponent(m_actorStatsComponent);
	m_elementEffectComponent->UpdateStats(false);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnEnemySpawned.Broadcast();
	eventHandler->OnBossDied.AddDynamic(this, &AAEnemyDrone::OnBossKilled);

	RequestRandomEquipment();
}

void AAEnemyDrone::SetPossibleElementIDs(TArray<int> possibleElementIDs)
{
	m_possibleElementIds = possibleElementIDs;
}

float AAEnemyDrone::PushPullResistance()
{
	return m_pushPullResistance;
}

void AAEnemyDrone::OnKeyHolderKill(int keyItemId)
{
	if (keyItemId < 0 || keyItemId == m_keyItemId)
	{
		UE_LOG(LogTemp, Display, TEXT("Destroyed Key holder"));
		Destroy();
	}
}

void AAEnemyDrone::OnKeyItemRequest(UObject* sender)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnKeyItemIdResponse.Broadcast(m_keyItemId, sender, this);
}

void AAEnemyDrone::EndPlay(EEndPlayReason::Type endPlayReason)
{

}

// Called every frame
void AAEnemyDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_stateComponent->GetCurrentState()->GetStateType() == CharacterStates::Paralysed)
	{
		GetCharacterMovement()->StopActiveMovement();
	}

	if (m_wantChangeModuleType)
	{
		m_currentModuleTryCount++;
		if (ChangeModuleType(m_newModuleType))
		{
			m_wantChangeModuleType = false;
			m_currentModuleTryCount = 0;
		}
		else if (m_currentModuleTryCount == m_moduleChangeTryCountMax)
		{
			m_wantChangeModuleType = false;
			m_currentModuleTryCount = 0;
		}
	}
}

void AAEnemyDrone::OnFirePrimaryPressed()
{
	m_weaponComponent->BeginShooting(0);
}

void AAEnemyDrone::OnFirePrimaryReleased()
{
	m_weaponComponent->StopFiring(0);
}

void AAEnemyDrone::OnFireSecondaryPressed()
{
	m_weaponComponent->BeginShooting(1);
	GetWorld()->GetTimerManager().ClearTimer(m_timeToNextShootHandle);
	GetWorld()->GetTimerManager().SetTimer(m_shootTimerHandle, this, &AAEnemyDrone::OnFirePrimaryReleased, 5.0f, false);
}

void AAEnemyDrone::OnFireSecondaryReleased()
{
	m_weaponComponent->StopFiring(1);
	GetWorld()->GetTimerManager().ClearTimer(m_shootTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(m_timeToNextShootHandle, this, &AAEnemyDrone::OnFirePrimaryPressed, 2.0f, false);
}

void AAEnemyDrone::ChangeLevel(int level)
{
	int prevLevel = m_actorStatsComponent->GetLevel();
	m_actorStatsComponent->SetLevel(level);
	m_healthComponent->UpdateStats();
	m_shieldComponent->UpdateStats();
	m_weaponComponent->UpdateStats();
	m_elementEffectComponent->UpdateStats(false);
	int levelDiff = level - prevLevel;
	for (int i = 1; i <= levelDiff; i++)
	{
		float expMult = float((prevLevel + i) * m_droppedExpLevelMult);
		m_droppedExp += float(m_droppedExpBase * expMult);
	}
}

void AAEnemyDrone::RequestRandomEquipment()
{
	// set modules
	for (size_t i = 0; i < m_Weapons.Num(); i++)
	{
		m_Weapons[i]->SetModuleIds(m_moduleIds);

		// set elements
		TArray<FElementIdSet> elementIdSets;
		elementIdSets.SetNum(m_elementSetCount);

		for (int e = 0; e < m_elementSetCount; e++)
		{
			elementIdSets[e].m_elementIds.SetNum(m_moduleIds.Num());

			for (int j = 0; j < m_moduleIds.Num(); j++)
			{
				// get random element id from list
				int randomIdx = rand() % m_possibleElementIds.Num();
				int elementId = m_possibleElementIds[randomIdx];

				elementIdSets[e].m_elementIds[j] = elementId;
			}
		}
		m_Weapons[i]->SetElementIdSets(elementIdSets);

		// weapon will automatically request items in first tick.

		UE_LOG(LogTemp, Display, TEXT("Drone requested random equipment"));
	}
}

void AAEnemyDrone::SetKeyHolder(int keyItemId)
{
	SetActorRelativeScale3D(FVector(2, 2, 2));
	m_keyItemId = keyItemId;
	m_isKeyHolder = true;

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnKeyItemIdRequest.AddDynamic(this, &AAEnemyDrone::OnKeyItemRequest);
	eventHandler->OnKeyHolderKill.AddDynamic(this, &AAEnemyDrone::OnKeyHolderKill);
}

void AAEnemyDrone::OnBossKilled()
{
	if (m_destroyOnKill)
		Destroy();
}

void AAEnemyDrone::DamageShield(float damage, EDamageEffectiveness effectiveness)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPlayerDealtDamage.Broadcast(damage);
	eventHandler->OnReceiveDamage.Broadcast(m_shieldComponent, -damage, effectiveness);

	if(m_shieldComponent)
		m_shieldComponent->UpdateShield(-damage);
	else
		UE_LOG(LogTemp, Error, TEXT("No component to damage, when hitting enemy %s: "), *GetName());
}

void AAEnemyDrone::DamageHealth(float damage, EDamageEffectiveness effectiveness)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPlayerDealtDamage.Broadcast(damage);
	eventHandler->OnReceiveDamage.Broadcast(m_shieldComponent, -damage, effectiveness);

	if(m_healthComponent)
		m_healthComponent->UpdateHealth(-damage);
	else
		UE_LOG(LogTemp, Error, TEXT("No component to damage, when hitting enemy %s: "), *GetName());
}

bool AAEnemyDrone::ChangeModuleType(int newType)
{
	UAModule* firingModule = m_weaponComponent->GetModule(newType);
	bool changedType = false;
	if (firingModule)
	{
		UMachineGunModule* machineGunMod = Cast<UMachineGunModule>(firingModule);
		if (machineGunMod)
		{
			machineGunMod->SetModuleType(newType);
			changedType = true;
		}
	}

	return changedType;
}