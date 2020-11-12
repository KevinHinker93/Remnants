// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AModule.h"
#include "MachineGunModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class REMNANTS_API UMachineGunModule : public UAModule
{
	GENERATED_BODY()
	
public:
	UMachineGunModule();
	// Plays sound when firing a bullet -> Base firing strat sound + element specific sound
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlaySoundOnFire"))
		void PlaySoundOnFire(FVector soundLoc, USoundBase* elementSound, USceneComponent* componentToAttachSound);

	void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) override;
	void StopFiring(UWorld* world) override;

	void SetModuleType(int moduleType);

private:
	float m_totalFiringSpeed;
	int m_moduleType;
};
