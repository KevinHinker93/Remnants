// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Items/AItem.h"
#include "Core/Items/AElement.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "AModule.generated.h"

class UAElement;
class UWeaponFiringStrategyBase;

USTRUCT(BlueprintType)
struct FModuleStat
{
	GENERATED_USTRUCT_BODY()
		FModuleStat() {}

	UPROPERTY()
	float m_totalVal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseGrowthVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_distributionFac;

	float CalcTotalStat(int rarityLevel, int maxLevelsToDistribute)
	{
		m_totalVal = 0;
		float statGain = m_baseGrowthVal * (rarityLevel / float(2));
		int levelsToDistribute = int(maxLevelsToDistribute * m_distributionFac);
		if (levelsToDistribute <= 0)
			m_totalVal = m_baseVal;
		else
			m_totalVal = m_baseVal + (statGain * levelsToDistribute);
		return m_totalVal;
	}
};

UCLASS(ABSTRACT, Blueprintable)
class REMNANTS_API UAModule : public UAItem
{
	GENERATED_BODY()

public:
	UAModule();

	// Sound to play each time we fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* m_FireSound;

	void SetLevelsToDistribute(int maxLevels);
	int GetModuleRarity() const;
	
	bool CanFire();
	UFUNCTION(BlueprintCallable)
	void SetCanFire(bool canFire);
	void EnableUse();

	void CalcModuleRarity();
	float GetDamageMult();
	float GetFiringSpeedDiv();
	float GetCriticalChance();
	float GetCriticalMult();

	virtual void FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType) { check(0 && "You must override this"); }
	virtual void StopFiring(UWorld* world) { check(0 && "You must override this"); }

	float GetTotalFiringSpeed();
	// Call function to check if module has a recoil or not
	// when firing
	virtual bool GetHasRecoil();
	virtual bool IsInUse();

	float GetOverheatingMult();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UWeaponFiringStrategyBase* GetFiringStrategy() { return m_firingStrat; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FTimerHandle GetCooldownTimerHandle() { return m_cooldownTimerHandle; }

protected:
	//class USceneComponent* m_root;

	///** Sphere collision component */
	//UPROPERTY(VisibleDefaultsOnly, Category = "Collision")
	//	class USphereComponent* m_collisionComp;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	//	class UStaticMeshComponent* m_mesh;

	//// Animation to play each time we fire a bullet
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	//	class UAnimMontage* m_fireAnimation;

	// --------------------------------------------------------------------------
	// Module Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		FModuleStat m_damageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		FModuleStat m_firingSpeedDiv;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		FModuleStat m_criticalChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		FModuleStat m_criticalDamageMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		float m_sizeMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		float m_randomMoveMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		float m_projSpeedMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Stats")
		float m_explosionRadiusMult;

	// -----------------------------------------------------------------------------
	// Module Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Properties")
		int m_moduleLevelsToDistribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Properties")
		bool m_hasRecoil;
	//In meter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Properties")
		float m_range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Settings")
		float m_overheatMult;

	// ----------------------------------------------------------------------------
	// Variables
	UWorld* m_world;
	FTimerHandle m_cooldownTimerHandle;
	bool m_canFire;
	int m_moduleRarity; // Level of rarity

	UPROPERTY()
		UWeaponFiringStrategyBase* m_firingStrat;
};
