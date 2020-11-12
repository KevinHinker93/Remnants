// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "ShotgunStrategy.generated.h"

UCLASS()
class REMNANTS_API UShotgunStrategy : public UWeaponFiringStrategyBase
{
	GENERATED_BODY()
	
public:
	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound) override;

	virtual void EndFire() override;

	UFUNCTION(BlueprintCallable)
		void SetRotOffset(float offset);
	UFUNCTION(BlueprintCallable)
		void SetNumProjectiles(int numProjectiles);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		float m_rotOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		int m_numCircles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		int m_numProjectiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		float m_randOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		bool m_projectileAtOrigin;

	virtual void OnSpawnedProjectile(AShootergameProjectile* proj);
};
