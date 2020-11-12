// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "WeaponFiringStrategyBase.generated.h"

class UAElement;
class USoundBase;
class USceneComponent;

USTRUCT(BlueprintType)
struct FFiringStrategyData
{
	GENERATED_USTRUCT_BODY()
	FFiringStrategyData() 
	{
		m_currentTimesUsed = 0;
	}

public:
	// Tells weapon if it should play firing animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_hasRecoil;
	// If attack will go on cooldown after being shot or when releasing mouse button
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_instantCooldown;
	// How often a projectile can be shot before it is on cooldown
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_useCountBeforeOnCooldown;
	// Time until next projectile can be fired before on cooldown
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_useIntervalBeforeCooldown;

	// How often firing occured
	int m_currentTimesUsed;
};

UCLASS(Blueprintable)
class REMNANTS_API UWeaponFiringStrategyBase : public UObject
{
	GENERATED_BODY()

public:
	// Plays sound when firing a bullet -> Base firing strat sound + element specific sound
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlaySoundOnFire"))
		void PlaySoundOnFire(FVector soundLoc, USoundBase* elementSound, USceneComponent* componentToAttachSound);

	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<class AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<class UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound); // module type = long range 0 - short range 1 // default = -1
	virtual void EndFire(){ check(0 && "You must override this"); };

	bool HasRecoil();
	bool InstantCooldown();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetUseCountBeforeOnCooldown();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetIntervalBeforeCooldown();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetStrategyFiringSpeedMult() const;

	bool IsInUse();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Parameters")
		ESpawnActorCollisionHandlingMethod m_collisionHandlingMethod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strategy Settings")
		FFiringStrategyData m_firingStrategyData; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strategy Settings")
		float m_firingSpeedMult;

	FActorSpawnParameters m_projectileSpawnParams;
	UWorld* m_world;

	bool m_isInUse;
};
