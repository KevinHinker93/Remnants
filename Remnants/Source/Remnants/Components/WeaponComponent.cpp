// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "../Core/Items/AModule.h"
#include "Animation/AnimInstance.h"
#include "../Core/Characters/ShootergameCharacter.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActorStatsComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "EventSystemHandlerSingleton.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	m_damageMult = 1.0f;
	m_currentOverheatVal = 0;
	m_maxOverheatingVal = 100;
	m_overheatedPercentage = 70;
	m_overheatingMult = 1.0f;
	m_damageMult = 1.0f;
	m_firingSpeedMult = 1.0f;

	m_canCooldown = false;
	m_overheated = false;
	m_activeModuleIdx = -1;
	m_activeElementSetIdx = 0;
	m_currentWeaponState = WeaponStates::Default;
}

// Starts shooting
void UWeaponComponent::BeginShooting(int moduleIdx)
{
	if (m_currentWeaponState != WeaponStates::Frozen)
	{
		if(m_activeModuleIdx != -1)
		{
			UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
			if (module && module->IsInUse())
				StopFiring(m_activeModuleIdx);
		}
		
		if (!m_overheated)
		{
			m_activeModuleIdx = moduleIdx;
			m_firing = true;
			m_canCooldown = false;
			GetWorld()->GetTimerManager().ClearTimer(m_overheatingTimerHandle);
		}
	}
}

// Tells module to stop firing
void UWeaponComponent::StopFiring(int moduleIdx)
{
	if (m_firing && m_activeModuleIdx == moduleIdx && m_currentWeaponState != WeaponStates::Frozen)
	{
		UAModule* module = GetModule(m_activeModuleIdx);
		if (!module)
		{
			UE_LOG(LogTemp, Warning, TEXT("No module equipped in slot %d"), m_activeModuleIdx);
			return;
		}
		module->StopFiring(GetWorld());
		m_firing = false;
		m_activeModuleIdx = -1;
		GetWorld()->GetTimerManager().SetTimer(m_overheatingTimerHandle, this, &UWeaponComponent::EnableCooldown, m_coolSpeed, false);
	}
}

void UWeaponComponent::SetModuleAtSlotIdx(int idx, UAModule* module)
{
	/*if(idx < 2)
		m_weaponModulesObject[idx] = module;*/
}

// Changes ElementSet of Weapon, Positive Value means next Set, Negative is previous Set
void UWeaponComponent::ChangeElementSet(float val)
{
	if(m_activeModuleIdx != -1)
	{
		UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
		if (module->IsInUse())
			return;
	}
	int arraySize = m_elementIdSets.Num();
	m_activeElementSetIdx = ((m_activeElementSetIdx + (int)val) + arraySize) % arraySize;
	OnWeaponChanged.Broadcast(this);
	UE_LOG(LogTemp, Display, TEXT("Current Element Set Index changed to: %i"), m_activeElementSetIdx);
}

void UWeaponComponent::SetElementSet(int val)
{
	if (m_activeModuleIdx != -1)
	{
		UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
		if (module->IsInUse())
			return;
	}
	int arraySize = m_elementIdSets.Num();
	if (val >= 0 && val < arraySize)
	{
		m_activeElementSetIdx = val;
		OnWeaponChanged.Broadcast(this);
		UE_LOG(LogTemp, Display, TEXT("Current Element Set Index changed to: %i"), m_activeElementSetIdx);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Warning: Element Set Index out of range"));
}

void UWeaponComponent::SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent)
{
	m_actorStatsComponent = actorStatsComponent;
}

void UWeaponComponent::UpdateStats()
{
	m_baseDamageVal = m_actorStatsComponent->GetDamageVal();
	m_firingSpeed = m_actorStatsComponent->GetFiringSpeed();
	m_maxOverheatingVal = m_actorStatsComponent->GetMaxOverheatVal();
	m_overheatingImpact = m_actorStatsComponent->GetOverheatFac();
	m_coolAmount = m_actorStatsComponent->GetOverheatRegenAmount();
	m_coolSpeed = m_actorStatsComponent->GetOverheatRegenTime();
}

float UWeaponComponent::GetFiringSpeedMult() const
{
	return m_firingSpeedMult;
}

void UWeaponComponent::SetFiringSpeedMult(float firingSpeedMult)
{
	m_firingSpeedMult = firingSpeedMult;
}

float UWeaponComponent::GetOverheatingMult() const
{
	return m_overheatingMult;
}

void UWeaponComponent::SetOverheatingMult(float overheatingMult)
{
	m_overheatingMult = overheatingMult;
}

float UWeaponComponent::GetDamageMult() const
{
	return m_damageMult;
}

void UWeaponComponent::SetDamageMult(float damageMult)
{
	m_damageMult = damageMult;
}

void UWeaponComponent::SetWeaponState(WeaponStates weaponState)
{
	m_currentWeaponState = weaponState;
}

void UWeaponComponent::OnExternalOverheat(AActor* actorToOverheat, float overheatingMult, int sourceModule)
{
	if (actorToOverheat != GetOwner())
		return;

	UAElement* element = GetElement(m_activeElementSetIdx, sourceModule);
	float overheatingVal = m_overheatingImpact * element->GetOverheatingMultiplier() * m_overheatingMult;
	UpdateOverheat(overheatingVal * overheatingMult);
}

void UWeaponComponent::OnExternalEndFiring(AActor* actorToEndFire, int sourceModule)
{
	if (actorToEndFire != GetOwner())
		return;
	if (sourceModule != m_activeModuleIdx)
		return;

	UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
		if (module->IsInUse())
			StopFiring(m_activeModuleIdx);
}

void UWeaponComponent::ResetWeaponState()
{
	m_currentWeaponState = WeaponStates::Default;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	m_canFire = false;
	m_firing = false;
	m_canCooldown = false;
	m_overheated = false;

	// disable firing in loading screen
	SetCanFire(false);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnItemChunkResponse.AddDynamic(this, &UWeaponComponent::ReceiveItemChunk);
	eventHandler->OnExternalOverheat.AddDynamic(this, &UWeaponComponent::OnExternalOverheat);
	eventHandler->OnExternalEndFiring.AddDynamic(this, &UWeaponComponent::OnExternalEndFiring);

	// disable firing in loading screen, enable firing after game is loaded
	eventHandler->OnSceneChangeStarted.AddDynamic(this, &UWeaponComponent::OnSceneChangeStarted);
	eventHandler->OnSceneChangeFinished.AddDynamic(this, &UWeaponComponent::OnSceneChangeFinished);
}

void UWeaponComponent::OnSceneChangeStarted(const FString& currentScene, const FString& sceneToLoad)
{
	SetCanFire(false);
}

void UWeaponComponent::SwapElements()
{
	if (GetModuleCount() < 2)
		return;
	auto a = GetElement(m_activeElementSetIdx, 1);
	auto b = GetElement(m_activeElementSetIdx, 0);
	SetElement(m_activeElementSetIdx, 1, b);
	SetElement(m_activeElementSetIdx, 0, a);
}

void UWeaponComponent::OnSceneChangeFinished(const FString& previousScene, const FString& loadedScene)
{
	SetCanFire(true);
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnItemChunkResponse.RemoveDynamic(this, &UWeaponComponent::ReceiveItemChunk);
	eventHandler->OnExternalOverheat.RemoveDynamic(this, &UWeaponComponent::OnExternalOverheat);
	eventHandler->OnExternalEndFiring.RemoveDynamic(this, &UWeaponComponent::OnExternalEndFiring);
}

void UWeaponComponent::SetCanFire(bool canFire)
{ 
	m_canFire = canFire;
	for (auto mod : m_modules)
		mod->SetCanFire(canFire);
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!m_requestedEquipment)
		RequestEquipment();
	if (!m_receivedEquipment)
		return;

	if (m_overheated)
	{
		for (auto module : m_modules)
		{
			if (!module)
			{
				UE_LOG(LogTemp, Warning, TEXT("No module equipped in slot %d"), 1);
				return;
			}
			if (module->IsInUse())
				module->StopFiring(GetWorld());
			m_firing = false;
			m_activeModuleIdx = -1;
		}
	}

	// Cannot shoot while frozen or with invalid equipment
	if (m_currentWeaponState == WeaponStates::Frozen) 
		return;

	if (m_activeModuleIdx != -1)
	{
		UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
		if (!module || m_activeModuleIdx < 0 || m_activeModuleIdx > m_modules.Num()) // no module
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon has no valid module at idx %d. Did you forget to set the module?"), m_activeModuleIdx);
			return;
		}

		if (module->CanFire() && m_firing && !m_overheated)
		{
			module->SetCanFire(false);
			
			// Fetch element (default element if invalid)
			UAElement* element = GetElement(m_activeElementSetIdx, m_activeModuleIdx);
			if (element == nullptr) // no possible element to fetch
			{
				UE_LOG(LogTemp, Error, TEXT("Weapon has no valid element or default element. Did you forget to set the elements?"));
				return;
			}

			UWorld* const World = GetWorld();

			if (World != NULL)
			{
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				ActorSpawnParams.OverrideLevel = GetOwner()->GetLevel();

				auto parentActorShootable = Cast<IAbleToShoot>(GetOwner());
				auto parentActor = GetOwner();

				FRotator spawnRot = GetComponentRotation();
				FVector spawnLoc = this->GetComponentLocation();
				float shooterDamageMult = parentActorShootable->GetDamageMultiplier();

				TArray<FName> owningActorTags = parentActor->Tags;
				checkf(owningActorTags.Num() > 0, TEXT("Shooter has no tags"));
				// Shot projectile, this also sets firing strategy of module
				module->FiringLogic(parentActor, spawnLoc, spawnRot, World, m_baseDamageVal, m_damageMult * shooterDamageMult, m_firingSpeed, m_firingSpeedMult, element, owningActorTags[0]);

				float overheatingVal = m_overheatingImpact * element->GetOverheatingMultiplier()
					* m_overheatingMult * module->GetOverheatingMult();
				UpdateOverheat(overheatingVal);

				if (parentActorShootable)
				{
					if (module->GetHasRecoil())
						parentActorShootable->WeaponFireAnimation();
				}
				else
					UE_LOG(LogTemp, Warning, TEXT("Actor is not able to shoot"));
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("World is null"));
		}
	}
	if (!m_firing && m_canCooldown && m_currentOverheatVal > 0)
		CoolDownWeapon();
}

// Add value to current overheating value, can be negative
void UWeaponComponent::UpdateOverheat(float value)
{
	m_currentOverheatVal = FMath::Max(0.0f, FMath::Min(m_currentOverheatVal + value, m_maxOverheatingVal));
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnOverheatChanged.Broadcast(this, m_currentOverheatVal / m_maxOverheatingVal);
	UE_LOG(LogTemp, VeryVerbose, TEXT("Add:: %f // OverheatingVal:: %f"), value, m_currentOverheatVal);

	if(m_activeModuleIdx != -1 && m_currentOverheatVal >= m_maxOverheatingVal)
	{
		UAModule* module = GetModule(m_activeModuleIdx); // Fetch active module
		StopFiring(m_activeModuleIdx);

		OnStartOverheat.Broadcast();
		m_overheated = true;
	}
}

void UWeaponComponent::ResetOverheat()
{
	m_currentOverheatVal = 0;
	m_activeModuleIdx = 0;
	m_overheated = false;
	OnEndOverheat.Broadcast();
	UpdateOverheat(0); // for events etc.
}

// Cools the weapon and disable cooldown
void UWeaponComponent::CoolDownWeapon()
{
	UpdateOverheat(- m_maxOverheatingVal * m_coolAmount);
	m_canCooldown = false;
	GetWorld()->GetTimerManager().SetTimer(m_overheatingTimerHandle, this, &UWeaponComponent::EnableCooldown, m_coolSpeed, false);
	if (m_overheated)
	{
		if (m_currentOverheatVal < m_maxOverheatingVal * (m_overheatedPercentage / 100))
		{
			m_overheated = false;
			OnEndOverheat.Broadcast();
		}
	}
}

// Enables cooldown
void UWeaponComponent::EnableCooldown()
{
	m_canCooldown = true;
	auto world = GetWorld();
	if (world != nullptr)
		world->GetTimerManager().ClearTimer(m_overheatingTimerHandle);
}

void UWeaponComponent::RequestEquipment()
{
	// builds item request based on IDs

	int moduleCount = m_moduleIds.Num();
	int elementSetCount = m_elementIdSets.Num();

	TArray<int> requestedItemIds;
	int itemCount = moduleCount + moduleCount * elementSetCount + 1;
	requestedItemIds.SetNum(moduleCount + moduleCount * elementSetCount + 1);

	// modules first
	for (int i = 0; i < moduleCount; i++)
		requestedItemIds[i] = m_moduleIds[i];

	// elements second
	for (int i = 0; i < elementSetCount; i++)
	{
		auto& elementSet = m_elementIdSets[i];

		// check if element set has correct size
		int elementCount = elementSet.m_elementIds.Num();
		if (elementCount != m_moduleIds.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Number of elements (%d) in set %d does not match number of modules (%d). Padding/clipping elements"),
				elementCount, i, m_moduleIds.Num());
		}

		// add elements (clamp num)
		for (int j = 0; j < m_moduleIds.Num(); j++)
		{
			int elementId = -1; // default id for "nothing"

			if (j < elementSet.m_elementIds.Num())
				elementId = elementSet.m_elementIds[j];

			int idx = moduleCount + i * moduleCount + j; // offset
			requestedItemIds[idx] = elementId;
		}
	}

	// add default element
	requestedItemIds[itemCount - 1] = m_defaultElementId;

	FString requestString = "";
	for (auto item : requestedItemIds)
	{
		requestString.AppendInt(item);
		requestString += " ";
	}

	m_requestedEquipment = true;
	UE_LOG(LogTemp, Display, TEXT("Weapon requested equipment: %s"), *requestString);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnItemChunkRequest.Broadcast(this, requestedItemIds);
}

void UWeaponComponent::ReceiveItemChunk(UObject* sender, TArray<UAItem*>& items)
{
	UE_LOG(LogTemp, Display, TEXT("Weapon received item chunk"));
	if (sender != this) return;

	if (m_receivedEquipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon has already received equipment. Please check active Ids!"));
	}

	// "unpack" item chunk
	// assure items are valid
	const int moduleCount = m_moduleIds.Num();
	const int elementSetCount = m_elementIdSets.Num();
	const int itemCount = moduleCount + moduleCount * elementSetCount + 1;

	if (items.Num() != itemCount)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon received invalid item array"));
		return;
	}

	// modules first
	for (int i = 0; i < moduleCount; i++)
	{
		auto item = items[i];
		auto module = Cast<UAModule>(item);

		if (!module) // item is not a module?
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon received invalid module"));
			return;
		}
		else
		{
			m_modules.Add(module);
		}
	}

	// elements second
	for (int i = 0; i < moduleCount * elementSetCount; i++)
	{
		int idx = i + moduleCount; // offset

		auto item = items[idx];
		auto element = Cast<UAElement>(item);

		if (item && !element) // item is not an element? accept nullptr, though (-> default element)
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon received invalid element"));
			return;
		}
		else
		{
			if (!item) // item is null?
				UE_LOG(LogTemp, Warning, TEXT("Weapon received 'null' element"));

			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
			eventHandler->OnReceivedElement.Broadcast(GetOwner(), element);
			
			m_elements.Add(element);
		}
	}

	// get default element
	{
		auto item = items[itemCount - 1];
		auto element = Cast<UAElement>(item);

		if (item && !element) // item is not an element? accept nullptr, though (-> default element)
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon received invalid default element"));
			return;
		}
		else
		{
			if (!item) // item is null?
				UE_LOG(LogTemp, Warning, TEXT("Weapon received 'null' default element"));
			m_defaultElement = element;
		}
	}

	m_receivedEquipment = true;
	m_activeModuleIdx = 0;
	m_activeElementSetIdx = 0;

	UE_LOG(LogTemp, Display, TEXT("Actor %s received equipment: "), *GetOwner()->GetName());
	for (auto module : m_modules)
		UE_LOG(LogTemp, Display, TEXT("Module: %s"), (module ? *module->GetName() : TEXT("null")));
	for (auto element : m_elements)
		UE_LOG(LogTemp, Display, TEXT("Element: %s"), (element ? *element->GetName() : TEXT("null")));
}

// execute before requesting equipment
void UWeaponComponent::SetModuleIds(TArray<int> moduleIds)
{
	m_moduleIds = moduleIds;
	OnWeaponChanged.Broadcast(this);
}

// execute before requesting equipment
void UWeaponComponent::SetElementIdSets(TArray<FElementIdSet> elementIdSets)
{
	m_elementIdSets = elementIdSets;
	OnWeaponChanged.Broadcast(this);
}

// execute before requesting equipment
void UWeaponComponent::SetDefaultElementId(int defaultElementId)
{
	m_defaultElementId = defaultElementId;
}

bool UWeaponComponent::HasReceivedEquipment()
{
	return m_receivedEquipment;
}

UAModule* UWeaponComponent::GetModule(int moduleIdx)
{
	if (moduleIdx < 0 || moduleIdx >= m_modules.Num())
		return nullptr;

	return m_modules[moduleIdx];
}

// Gets an element in the 1d array by set idx and module idx
UAElement* UWeaponComponent::GetElement(int elementSetIdx, int moduleIdx)
{
	int idx = elementSetIdx * m_modules.Num(); // gets correct element set in 1d array
	idx += moduleIdx; // gets correct element in element set

	// return default element if index is out of range or if element is nullptr
	if (idx < 0 || idx >= m_elements.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Element idx out of range. Returning default element"));
		return m_defaultElement;
	}
	else if (!m_elements[idx])
	{
		UE_LOG(LogTemp, Warning, TEXT("Element is null. Returning default element"));
		return m_defaultElement;
	}
	else
	{
		return m_elements[idx];
	}
}

// Sets module. Returns existing entry, if present.
UAModule* UWeaponComponent::SetModule(int moduleIdx, UAModule* module)
{
	if (moduleIdx < 0 || moduleIdx >= m_modules.Num())
		return nullptr;

	else if (m_modules[moduleIdx])
		UE_LOG(LogTemp, Display, TEXT("Module slot %d is already occupied"), moduleIdx);

	auto tempModule = m_modules[moduleIdx];
	m_modules[moduleIdx] = module;
	OnWeaponChanged.Broadcast(this);
	return tempModule;
}

// Sets an element in the 1d array by set idx and module idx. Returns existing entry, if present.
UAElement* UWeaponComponent::SetElement(int elementSetIdx, int moduleIdx, UAElement* element)
{
	int idx = elementSetIdx * m_modules.Num(); // gets correct element set in 1d array
	idx += moduleIdx; // gets correct element in element set

	if (idx < 0 || idx >= m_elements.Num())
		return nullptr;

	else if (m_elements[idx])
		UE_LOG(LogTemp, Display, TEXT("Element slot %d in weapon set %d is already occupied"), moduleIdx, elementSetIdx);
	
	auto tempElement = m_elements[idx];
	m_elements[idx] = element;
	OnWeaponChanged.Broadcast(this);
	return tempElement;
}

int UWeaponComponent::GetModuleCount()
{
	return m_modules.Num();
}

int UWeaponComponent::GetElementCount()
{
	return m_elements.Num();
}

int UWeaponComponent::GetElementSetCount()
{
	return m_modules.Num() > 0 ? (m_elements.Num() / m_modules.Num()) : 0;
}