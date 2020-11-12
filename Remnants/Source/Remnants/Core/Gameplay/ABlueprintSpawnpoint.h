// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../SpawnStruct.h"
#include "../Structural/WeightedRandom.h"
#include "../Structural/BlueprintProbabilityStruct.h"
#include "ABlueprintSpawnpoint.generated.h"

UCLASS(Blueprintable, Abstract)
class REMNANTS_API ABlueprintSpawnpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlueprintSpawnpoint();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type reason) override;
	virtual void Tick(float deltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		bool m_activated;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		TArray<FBlueprintProbabilityStruct> m_blueprintProbabilities;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		bool m_spawnOnBegin;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		bool m_waitOneFrame;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		bool m_respawn;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		float m_spawnInterval;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		int m_totalSpawnCount; // -1 = infinite
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		int m_globalCap;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		FString m_spawnGroupTag;
	UPROPERTY(EditAnywhere, Category = "Spawn Options")
		bool m_deactivateOnBossDeath;

	void SpawnRandom();

	WeightedRandom m_weightedRandom;
public:	
	virtual AActor* Spawn(int idx);
private:
	UFUNCTION()
		void OnSpawnpointObjectSpawned(const FString& tag);
	UFUNCTION()
		void OnSpawnpointObjectDestroyed(const FString& tag);
	UFUNCTION()
		void OnBossDeath();
	UFUNCTION()
		void OnKeyHolderDied(int keyItemId);
	UFUNCTION()
		void OnSpawnPointActivated(const FString& tag, bool activated);

	FString m_internalSpawnGroupTag;
	int m_spawnCount;
	int m_currentGlobalSpawnCount;
	FTimerHandle m_timer;
	void SpawnCallback();
};