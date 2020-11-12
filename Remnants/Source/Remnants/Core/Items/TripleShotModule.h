// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AModule.h"
#include "TripleShotModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class REMNANTS_API UTripleShotModule : public UAModule
{
	GENERATED_BODY()
	
public:
	UTripleShotModule();
	void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) override;
	void StopFiring(UWorld* world) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triple Shot Settings")
		float m_rotOffset;

private:
	bool m_projectileAtOrigin;
};
