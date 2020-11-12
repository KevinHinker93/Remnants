// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/ShotgunStrategy.h"
#include "HomingShotgunStrategy.generated.h"

UCLASS()
class REMNANTS_API UHomingShotgunStrategy : public UShotgunStrategy
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void SetHomingTargetActor(AActor* target, float magnitude);

	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
		float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
		TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
		FName shooterType, int moduleType, USoundBase* elementSound) override;

	virtual void EndFire() override;

protected:
	virtual void OnSpawnedProjectile(AShootergameProjectile* proj) override;

private:
	UPROPERTY()
		AActor* m_homingTargetActor;
	float m_homingMagnitude;
};
