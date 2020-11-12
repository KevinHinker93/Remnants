// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AModule.h"
#include "ShootergameProjectile.h"
#include "ShootergameBeam.h"
#include "BeamModule.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTS_API UBeamModule : public UAModule
{
	GENERATED_BODY()

public:
	UBeamModule();
	void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) override;
	void StopFiring(UWorld* world) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing")
		bool m_firing;

private:
	class AShootergameBeam* m_beam;
};
