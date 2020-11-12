// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABlueprintSpawnpoint.h"
#include "EnemySpawnpoint.generated.h"

class UParticleSystem;

UCLASS()
class REMNANTS_API AEnemySpawnpoint : public ABlueprintSpawnpoint
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;
protected:
	UPROPERTY(EditAnywhere, Category="Spawned Drone Options")
		int m_minLevel;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		int m_maxLevel;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		bool m_increaseLevelPerSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		int m_minLevelIncreaseAmount;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		int m_maxLevelIncreaseAmount;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		bool m_canSpawnKeyHolder;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		int m_keyItemId;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		float m_keyHolderScale;
	UPROPERTY(EditAnywhere)
		UParticleSystem* m_keyHolderParticleSystem;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		float m_keyHolderHealthScale;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		float m_keyHolderShieldScale;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		float m_keyHolderDamageScale;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		bool m_overrideDroneItemIds;
	UPROPERTY(EditAnywhere, Category = "Spawned Drone Options")
		TArray<int> m_possibleItemIds;

	UFUNCTION()
		void OnKeyHolderSpawned(class AAEnemyDrone* keyHolder);
	UFUNCTION()
		void OnSpawnpointLevelChanged(int newLevel);
	UFUNCTION()
		void OnKeyHolderKilled(int keyItemId);

	UFUNCTION()
		void OnBossKilled();

	virtual AActor* Spawn(int idx) override;

	bool m_keyHolderExists;
};
