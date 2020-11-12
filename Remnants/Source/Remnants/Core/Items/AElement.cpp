// Fill out your copyright notice in the Description page of Project Settings.

#include "AElement.h"
#include "ShootergameProjectile.h"
#include "ShootergameBeam.h"

UAElement::UAElement()
{
	m_damageTypeClasses.SetNum(3);
	m_elementWeaponFiringStrategy.SetNum(2);
	m_level = 1;
}

TSubclassOf<AShootergameProjectile> UAElement::GetProjectile()
{
	return m_projectileClass;
}

TSubclassOf<AShootergameBeam> UAElement::GetBeam()
{
	return m_BeamClass;
}

TSubclassOf<UElementDamageType> UAElement::GetDamageTypeClass()
{
	return m_damageTypeClasses[m_level - 1];
}
TSubclassOf<UWeaponFiringStrategyBase> UAElement::GetElementWeaponFiringStrategy(int idx)
{
	checkf(idx >= 0 && idx < 2, TEXT("invalid idx for elementWeaponFiringStrategy access %d, needs to be between 0-1"), idx);
	return m_elementWeaponFiringStrategy[idx];
}

int UAElement::GetElementLevel()
{
	return m_level;
}

float UAElement::GetElementMinRange(int moduleType)
{
	//m_minRanges.
	if (m_minRanges.Num() - 1 >= moduleType)
	{
		return m_minRanges[moduleType];
	}
	else
		return 0;
}

float UAElement::GetOverheatingMultiplier()
{
	RecalculateStatsIfDirty();
	return m_overheatingMultiplier.m_totalVal;
}

float UAElement::GetElementalFiringSpeedDiv()
{
	RecalculateStatsIfDirty();
	return m_elementalFiringSpeedDiv.m_totalVal;
}

float UAElement::GetElementalDamageMult()
{
	RecalculateStatsIfDirty();
	return m_elementalDamageMultiplier.m_totalVal;
}

float UAElement::GetElementalProjectileSpeedMult()
{
	RecalculateStatsIfDirty();
	return m_elementalProjectileSpeedMult.m_totalVal;
}

void UAElement::SetLevel(int level)
{
	m_level = level;
	m_statsDirty = true;
}

USoundBase* UAElement::GetElementSound()
{
	return m_sound;
}

void UAElement::RecalculateStatsIfDirty()
{
	if (m_statsDirty)
	{
		m_elementalDamageMultiplier.CalcTotalStat(m_level);
		m_elementalProjectileSpeedMult.CalcTotalStat(m_level);
		m_elementalFiringSpeedDiv.CalcTotalStat(m_level);
		m_overheatingMultiplier.CalcTotalStat(m_level);

		m_statsDirty = false;
	}
}