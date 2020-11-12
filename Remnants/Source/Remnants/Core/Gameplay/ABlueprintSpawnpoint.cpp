// Fill out your copyright notice in the Description page of Project Settings.

#include "ABlueprintSpawnpoint.h"
#include "../../EventSystemHandlerSingleton.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"

ABlueprintSpawnpoint::ABlueprintSpawnpoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlueprintSpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(m_timer, this, &ABlueprintSpawnpoint::SpawnCallback, m_spawnInterval, false);

	m_internalSpawnGroupTag = FString("Spawn Group: ").Append(m_spawnGroupTag);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSpawnpointObjectSpawned.AddDynamic(this, &ABlueprintSpawnpoint::OnSpawnpointObjectSpawned);
	eventHandler->OnSpawnpointObjectDestroyed.AddDynamic(this, &ABlueprintSpawnpoint::OnSpawnpointObjectDestroyed);
	eventHandler->OnSpawnPointActivated.AddDynamic(this, &ABlueprintSpawnpoint::OnSpawnPointActivated);
	if (m_deactivateOnBossDeath)
	{
		eventHandler->OnBossDied.AddDynamic(this, &ABlueprintSpawnpoint::OnBossDeath);
		eventHandler->OnKeyHolderDied.AddDynamic(this, &ABlueprintSpawnpoint::OnKeyHolderDied);
	}

	// copy probabilities to int array
	TArray<float> probabilities;
	probabilities.SetNum(m_blueprintProbabilities.Num());
	for (int i = 0; i < probabilities.Num(); i++)
		probabilities[i] = m_blueprintProbabilities[i].m_probability;
	m_weightedRandom.SetProbabilities(probabilities);

	if (m_spawnOnBegin && !m_waitOneFrame)
		SpawnRandom();
}

void ABlueprintSpawnpoint::EndPlay(EEndPlayReason::Type reason)
{
	GetWorld()->GetTimerManager().ClearTimer(m_timer);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSpawnpointObjectSpawned.RemoveDynamic(this, &ABlueprintSpawnpoint::OnSpawnpointObjectSpawned);
	eventHandler->OnSpawnpointObjectDestroyed.RemoveDynamic(this, &ABlueprintSpawnpoint::OnSpawnpointObjectDestroyed);
	eventHandler->OnSpawnPointActivated.RemoveDynamic(this, &ABlueprintSpawnpoint::OnSpawnPointActivated);
	if (m_deactivateOnBossDeath)
	{
		eventHandler->OnBossDied.RemoveDynamic(this, &ABlueprintSpawnpoint::OnBossDeath);
		eventHandler->OnKeyHolderDied.RemoveDynamic(this, &ABlueprintSpawnpoint::OnKeyHolderDied);
	}
}

void ABlueprintSpawnpoint::OnSpawnpointObjectSpawned(const FString& tag)
{
	if (tag != m_internalSpawnGroupTag) return;
	m_currentGlobalSpawnCount++;
}

void ABlueprintSpawnpoint::OnSpawnpointObjectDestroyed(const FString& tag)
{
	if (tag != m_internalSpawnGroupTag) return;
	m_currentGlobalSpawnCount--;
}

void ABlueprintSpawnpoint::SpawnCallback()
{
	SpawnRandom();
	GetWorld()->GetTimerManager().ClearTimer(m_timer);
	if (m_respawn)
		GetWorld()->GetTimerManager().SetTimer(m_timer, this, &ABlueprintSpawnpoint::SpawnCallback, m_spawnInterval, false);
}

void ABlueprintSpawnpoint::Tick(float deltaTime)
{
	if (m_spawnOnBegin && m_waitOneFrame)
	{
		SpawnRandom();
		m_waitOneFrame = false;
	}
}

void ABlueprintSpawnpoint::OnSpawnPointActivated(const FString& tag, bool activated)
{
	if (tag == m_spawnGroupTag || tag.Len() < 1)
		m_activated = activated;
}

AActor* ABlueprintSpawnpoint::Spawn(int idx)
{
	if (idx < 0 || idx >= m_blueprintProbabilities.Num() // invalid idx
		|| !m_activated  // spawnpoint deactivated
		|| m_currentGlobalSpawnCount >= m_globalCap // global cap reached
		|| (m_totalSpawnCount > 0 && m_spawnCount >= m_totalSpawnCount) // all drones spawned
		)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not spawn object with idx %d"), idx);
		return nullptr;
	}
	
	if (m_blueprintProbabilities[idx].m_blueprint)
	{
		FActorSpawnParameters params{};
		params.OverrideLevel = GetLevel();

		auto actor = GetWorld()->SpawnActor<AActor>(m_blueprintProbabilities[idx].m_blueprint, params);
		actor->SetActorTransform(this->GetActorTransform());
		actor->Tags.Add(FName(*m_internalSpawnGroupTag));

		m_spawnCount++;

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnSpawnpointObjectSpawned.Broadcast(m_internalSpawnGroupTag);

		return actor;
	}

	return nullptr;
}

void ABlueprintSpawnpoint::SpawnRandom()
{
	// get random blueprint
	int idx = m_weightedRandom.ReturnRandomIdx();
	Spawn(idx);
}

void ABlueprintSpawnpoint::OnBossDeath()
{
	m_activated = false;
}

void ABlueprintSpawnpoint::OnKeyHolderDied(int keyItemId)
{
	m_activated = false;
}