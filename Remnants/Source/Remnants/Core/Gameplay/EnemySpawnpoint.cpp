// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawnpoint.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/ShieldComponent.h"
#include "../../Components/WeaponComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "../../EventSystemHandlerSingleton.h"

void AEnemySpawnpoint::OnBossKilled()
{
	Destroy();
}

AActor* AEnemySpawnpoint::Spawn(int idx)
{
	int level = FMath::RandRange(m_minLevel, m_maxLevel);

	if (m_increaseLevelPerSpawn)
	{
		m_minLevel += m_minLevelIncreaseAmount;
		m_maxLevel += m_maxLevelIncreaseAmount;
	}

	auto actor = Super::Spawn(idx);
	auto enemy = Cast<AAEnemyDrone>(actor);

	if (enemy)
	{
		enemy->ChangeLevel(level);

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		auto eventFacade = eventHandler->GetEventFacade();
		FItemStruct itemInfo = eventFacade->GetItemInfo(m_keyItemId);
		UTexture2D* keyItemImage = itemInfo.m_displayImage;
		FText keyName = itemInfo.m_name;
		FString questText = "Find ";
		if (m_canSpawnKeyHolder && !m_keyHolderExists)
		{
			if (!m_keyHolderExists)
			{
				enemy->SetKeyHolder(m_keyItemId);
				enemy->SetActorScale3D(FVector(m_keyHolderScale, m_keyHolderScale, m_keyHolderScale));
				if (m_keyHolderHealthScale > 0.0f)
				{
					enemy->SetFloatVal(EntityStats::MaxHealth, m_keyHolderHealthScale);
					UHealthComponent* healthComp = Cast<UHealthComponent>(enemy->GetComponentByClass(UHealthComponent::StaticClass()));
					if (healthComp)
						healthComp->SetHealthToMax();
				}
				if (m_keyHolderShieldScale > 0.0f)
				{
					enemy->SetFloatVal(EntityStats::MaxShield, m_keyHolderShieldScale);
					UShieldComponent* shieldComp = Cast<UShieldComponent>(enemy->GetComponentByClass(UShieldComponent::StaticClass()));
					if (shieldComp)
						shieldComp->SetShieldToMax();
				}
				if(m_keyHolderDamageScale > 0.0f)
					enemy->SetFloatVal(EntityStats::Damage, m_keyHolderDamageScale);
				if (m_keyHolderParticleSystem)
				{
					auto particles = UGameplayStatics::SpawnEmitterAttached(m_keyHolderParticleSystem, enemy->GetRootComponent(), NAME_None,
						enemy->GetActorLocation(), FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
					particles->SetRelativeLocation(FVector(0, 0, 0));
				}

				enemy->SetFloatVal(EntityStats::FiringStrat, 0.0f);

				/*UGameplayStatics::SpawnEmitterAttached(m_keyHolderParticleSystem, enemy->GetRootComponent(),
					FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy)
				questText.Append(keyName.ToString());*/
				eventHandler->OnDisplayQuest.Broadcast(keyItemImage, FText::FromString(questText), false);
			}
		}

		if (m_overrideDroneItemIds)
		{
			enemy->SetPossibleElementIDs(m_possibleItemIds);
			enemy->RequestRandomEquipment();
		}
		return enemy;
	}

	return nullptr;
}

void AEnemySpawnpoint::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSpawnpointLevelChanged.AddDynamic(this, &AEnemySpawnpoint::OnSpawnpointLevelChanged);
	eventHandler->OnBossDied.AddDynamic(this, &AEnemySpawnpoint::OnBossKilled);
	if (m_canSpawnKeyHolder)
	{
		eventHandler->OnKeyHolderKill.AddDynamic(this, &AEnemySpawnpoint::OnKeyHolderKilled);
		eventHandler->OnKeyHolderDied.AddDynamic(this, &AEnemySpawnpoint::OnKeyHolderKilled);
	}
}

void AEnemySpawnpoint::EndPlay(EEndPlayReason::Type reason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSpawnpointLevelChanged.RemoveDynamic(this, &AEnemySpawnpoint::OnSpawnpointLevelChanged);
	if (m_canSpawnKeyHolder)
	{
		eventHandler->OnKeyHolderKill.RemoveDynamic(this, &AEnemySpawnpoint::OnKeyHolderKilled);
		eventHandler->OnKeyHolderDied.RemoveDynamic(this, &AEnemySpawnpoint::OnKeyHolderKilled);
	}
}

void AEnemySpawnpoint::OnKeyHolderSpawned(AAEnemyDrone* keyHolder)
{
	m_keyHolderExists = true;
}

void AEnemySpawnpoint::OnSpawnpointLevelChanged(int newLevel)
{
	int range = newLevel / 8;
	m_minLevel = FMath::Max(1, newLevel - range);
	m_maxLevel = newLevel + range;
}

void AEnemySpawnpoint::OnKeyHolderKilled(int keyItemId)
{
	if (keyItemId < 0 || keyItemId == m_keyItemId)
	{
		m_activated = false;

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		auto eventFacade = eventHandler->GetEventFacade();
		FItemStruct itemInfo = eventFacade->GetItemInfo(m_keyItemId);
		UTexture2D* keyItemImage = itemInfo.m_displayImage;
		FText keyName = itemInfo.m_name;
		FString questText = "Find ";

		//questText.Append(keyName.ToString());
		eventHandler->OnDisplayQuest.Broadcast(keyItemImage, FText::FromString(questText), true);

	}
}