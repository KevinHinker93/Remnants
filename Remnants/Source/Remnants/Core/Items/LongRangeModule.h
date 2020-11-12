// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AModule.h"
#include "ModuleTypeEnum.h"
#include "LongRangeModule.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTS_API ULongRangeModule : public UAModule
{
	GENERATED_BODY()
	
public:
	ULongRangeModule();
	void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) override;
	void StopFiring(UWorld* world) override;
	bool GetHasRecoil() override;
	bool IsInUse() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Settings")
		ModuleType m_moduleType;

private:
	float m_totalFiringSpeed;
};
