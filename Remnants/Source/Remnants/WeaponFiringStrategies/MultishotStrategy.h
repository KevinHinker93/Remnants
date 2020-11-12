// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "TimerManager.h"
#include "MultishotStrategy.generated.h"

UCLASS()
class REMNANTS_API UMultishotStrategy : public UWeaponFiringStrategyBase
{
	GENERATED_BODY()
	
public:
	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound) override;

	virtual void EndFire() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multishot Settings")
		int m_maxMultipleShotCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multishot Settings")
		float m_distanceToNextProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multishot Settings")
		float m_multipleShotInterval;

private:
	FTimerDelegate m_timerDelegate;
	FTimerHandle m_multipleShotIntervalTimerHandle;

	int m_multipleShotCount; // How many projectiles remain to fire

	UFUNCTION()
	void OnMultipleShot(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound);
};
