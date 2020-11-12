// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AModule.h"
#include "ShotgunModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class REMNANTS_API UShotgunModule : public UAModule
{
	GENERATED_BODY()

public:
	UShotgunModule();
		// Plays sound when firing a bullet -> Base firing strat sound + element specific sound
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlaySoundOnFire"))
		void PlaySoundOnFire(FVector soundLoc, USoundBase* elementSound, USceneComponent* componentToAttachSound);

	void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) override;
	void StopFiring(UWorld* world) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		float m_rotOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		int m_numCircles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		int m_numProjectiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shotgun Settings")
		float m_randOffset;

private:
	bool m_projectileAtOrigin;
	float m_totalFiringSpeed;
};
