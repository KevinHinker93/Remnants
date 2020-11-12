// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "SniperStrategy.generated.h"

UCLASS()
class REMNANTS_API USniperStrategy : public UWeaponFiringStrategyBase
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sniper Settings")
		bool m_useOverheatAsMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sniper Settings")
		float m_damageMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sniper Settings")
		float m_overheatMult;
};
