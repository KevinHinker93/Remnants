// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "ThrowerStrategy.generated.h"

class AShootergameBeam;

UCLASS()
class REMNANTS_API UThrowerStrategy : public UWeaponFiringStrategyBase
{
	GENERATED_BODY()
	
public:
	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound) override;
	virtual void EndFire() override;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam")
	//	TSubclassOf<AShootergameBeam> m_beamClass;

private:
	AShootergameBeam* m_beam;
	AActor* m_shooter;
};
