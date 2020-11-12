// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AItem.h"
#include "ShootergameProjectile.h"
#include "../../CraftingInternalLogic.h"
#include "AElement.generated.h"

class UWeaponFiringStrategyBase;
class USoundBase;

USTRUCT(BlueprintType)
struct FElementStat
{
	GENERATED_USTRUCT_BODY()
		FElementStat() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_totalVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseGrowthVal;

	float CalcTotalStat(int elementLevel)
	{
		m_totalVal = m_baseVal;
		for (int i = 0; i < elementLevel; i++)
		{
			m_totalVal += m_baseGrowthVal * i;
		}
		return m_totalVal;
	}
};

UCLASS(Blueprintable)
class REMNANTS_API UAElement : public UAItem
{
	GENERATED_BODY()
	
public:
	UAElement();
	// Returns the projectile with the desired element
	TSubclassOf<class AShootergameProjectile> GetProjectile();
	// Returns the beam with the desired element
	TSubclassOf<class AShootergameBeam> GetBeam();
	// Returns the damageType class
	TSubclassOf<class UElementDamageType> GetDamageTypeClass();
	// Returns the firing logic based on long range or short range module
	TSubclassOf<UWeaponFiringStrategyBase> GetElementWeaponFiringStrategy(int idx); // idx = long range 0 - short range 1
	int GetElementLevel();
	float GetElementMinRange(int moduleType);

	// Returns element impact on overheating property
	UFUNCTION(BlueprintPure)
		float GetOverheatingMultiplier();
	// Returns element impact on firing speed property
	UFUNCTION(BlueprintPure)
		float GetElementalFiringSpeedDiv();
	// Returns element impact on damage property
	UFUNCTION(BlueprintPure)
		float GetElementalDamageMult();
	// Returns element impact on projectile movement speed property
	UFUNCTION(BlueprintPure)
		float GetElementalProjectileSpeedMult();

	void SetLevel(int level);
	USoundBase* GetElementSound();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		TSubclassOf<class AShootergameProjectile> m_projectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam")
		TSubclassOf<class AShootergameBeam> m_BeamClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		TArray<TSubclassOf<class UElementDamageType>> m_damageTypeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		TArray<TSubclassOf<UWeaponFiringStrategyBase>> m_elementWeaponFiringStrategy;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Info")
		FName m_elementName;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Sound")
		USoundBase* m_sound;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Element Info")
		int m_level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Stats")
		FElementStat m_elementalDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Stats")
		FElementStat m_elementalFiringSpeedDiv;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Stats")
		FElementStat m_overheatingMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Stats")
		FElementStat m_randomMovementValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Stats")
		FElementStat m_elementalProjectileSpeedMult;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Requirements")
		TArray<int> m_craftingGemIds;*/

		//In meter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Properties")
		TArray<float> m_minRanges;

	void RecalculateStatsIfDirty();
	bool m_statsDirty = true;
};
