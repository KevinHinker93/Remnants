// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorStatsComponent.generated.h"

USTRUCT()
struct FStatFormulas
{
	GENERATED_USTRUCT_BODY()
		FStatFormulas() {}

	float MainStatFormular(int level, float baseVal, float baseGrowthVal, float distributionFac)
	{
		float distributedLevel = float(level) * distributionFac;

		if (distributedLevel == 0)
			return baseVal;

		float distributedLevelFac = FMath::Pow(distributedLevel, 2) / 16;
		float baseGrowFac = FMath::Pow(baseGrowthVal, 2) / 3;
		float baseStatWeight = FMath::Pow(baseVal, 2) / 1000;

		return distributedLevelFac * baseGrowFac + baseStatWeight + baseVal;
	}

	float SecondaryStatFormular(int level, float baseVal, float baseGrowthVal, float distributionFac)
	{
		float distributedLevel = float(level) * distributionFac;

		if (distributedLevel == 0)
			return baseVal;

		return float(distributedLevel * baseGrowthVal + baseVal);
	}
};

USTRUCT(BlueprintType)
struct FGrowingStat
{
	GENERATED_USTRUCT_BODY()
		FGrowingStat() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
		float m_baseGrowthVal;
};

USTRUCT(BlueprintType)
struct FHPStat
{
	GENERATED_USTRUCT_BODY()
		FHPStat() {}

	UPROPERTY()
		FStatFormulas m_formulars; // Each stat hold the formular to calculate the stat

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_hp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_elementEffectResistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_elementEffectDepleteAmount; // Fix stat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_elementEffectDepleteTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_distributionFac;

	int CalcHPVal(int level)
	{
		return int(m_formulars.MainStatFormular(level, m_hp.m_baseVal,m_hp.m_baseGrowthVal, m_distributionFac));
	}

	int CalcPlayerElementEffectResistanceVal(int level)
	{
		return int(m_formulars.SecondaryStatFormular(level, m_elementEffectResistance.m_baseVal, m_elementEffectResistance.m_baseGrowthVal, m_distributionFac));
	}

	int CalcEnemyElementEffectResistanceVal(int level)
	{
		return int(m_formulars.MainStatFormular(level, m_elementEffectResistance.m_baseVal, m_elementEffectResistance.m_baseGrowthVal, m_distributionFac));
	}

	float CalcElementEffectDepleteTime(int level)
	{
		return m_formulars.SecondaryStatFormular(level, m_elementEffectDepleteTime.m_baseVal, m_elementEffectDepleteTime.m_baseGrowthVal, m_distributionFac);
	}
};

USTRUCT(BlueprintType)
struct FShieldStat
{
	GENERATED_USTRUCT_BODY()
		FShieldStat() {}

	UPROPERTY()
		FStatFormulas m_formulars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_shield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_shieldRegenAmount; // Fix stat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_shieldRegenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_distributionFac;

	int CalcShieldVal(int level)
	{
		return int(m_formulars.MainStatFormular(level, m_shield.m_baseVal, m_shield.m_baseGrowthVal, m_distributionFac));
	}

	float CalcShieldRegenTime(int level)
	{
		return m_formulars.SecondaryStatFormular(level, m_shieldRegenTime.m_baseVal, m_shieldRegenTime.m_baseGrowthVal, m_distributionFac);
	}
};

USTRUCT(BlueprintType)
struct FDamageStat
{
	GENERATED_USTRUCT_BODY()
		FDamageStat() {}

	UPROPERTY()
		FStatFormulas m_formulars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_firingSpeed; // Fix stat

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_maxOverheatVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_overheatVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_overheatRegenAmount; // Fix stat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		FGrowingStat m_overheatRegenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float m_distributionFac;

	int CalcDamageVal(int level)
	{
		return int(m_formulars.MainStatFormular(level, m_damage.m_baseVal, m_damage.m_baseGrowthVal, m_distributionFac));
	}

	int CalcMaxOverheatVal(int level)
	{
		return int(m_formulars.SecondaryStatFormular(level, m_maxOverheatVal.m_baseVal, m_maxOverheatVal.m_baseGrowthVal, m_distributionFac));
	}

	int CalcOverheatVal(int level)
	{
		return int(m_formulars.SecondaryStatFormular(level, m_overheatVal.m_baseVal, m_overheatVal.m_baseGrowthVal, m_distributionFac));
	}

	float CalcOverheatRegenTime(int level)
	{
		return m_formulars.SecondaryStatFormular(level, m_overheatRegenTime.m_baseVal, m_overheatRegenTime.m_baseGrowthVal, m_distributionFac);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UActorStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorStatsComponent();

	int GetLevel() const;
	// Sets level for actor and updates stats
	void SetLevel(int level);

	UFUNCTION(BlueprintPure, Category = "Stats")
	int GetHPVal();

	UFUNCTION(BlueprintPure, Category = "Stats")
	int GetShieldVal();
	float GetShieldRegenAmount();
	float GetShieldRegenTime();

	int GetPlayerElementEffectResistanceVal();
	int GetEnemyElementEffectResistanceVal();
	float GetElementEffectDepleteAmount();
	float GetElementEffectDepleteTime();

	int GetMaxOverheatVal();
	int GetOverheatFac();
	float GetOverheatRegenAmount();
	float GetOverheatRegenTime();

	UFUNCTION(BlueprintPure, Category = "Stats")
	int GetDamageVal();
	float GetFiringSpeed();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetShieldDistributionFac(float distributionFac)
	{
		m_shieldStat.m_distributionFac = distributionFac;
	}

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetAttackDistributionFac(float distributionFac)
	{
		m_damageStat.m_distributionFac = distributionFac;
	}

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetHealthDistributionFac(float distributionFac)
	{
		m_hpStat.m_distributionFac = distributionFac;
	}

	float GetHealthDistributionFac() { return m_hpStat.m_distributionFac; }
	float GetAttackDistributionFac() { return m_damageStat.m_distributionFac; }
	float GetShieldDistributionFac() { return m_shieldStat.m_distributionFac; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int m_level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP Stat")
		FHPStat m_hpStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Stat")
		FShieldStat m_shieldStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Stat")
		FDamageStat m_damageStat;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool m_levelDirty = true;
	bool m_firstFrame = true;
};




