// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "HorizontalMultishotStrategy.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTS_API UHorizontalMultishotStrategy : public UWeaponFiringStrategyBase
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float m_rotOffset;
	// Map that holds number of fired projectiles based on element level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TMap<int, int> m_projectileCountsAtElementLvl; 
};
