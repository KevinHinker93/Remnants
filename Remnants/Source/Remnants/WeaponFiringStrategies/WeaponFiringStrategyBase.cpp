// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponFiringStrategyBase.h"
#include "../Core/Items/ShootergameProjectile.h"

void UWeaponFiringStrategyBase::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound)
{
	if (!m_world)
		m_world = world;
	m_projectileSpawnParams.SpawnCollisionHandlingOverride = m_collisionHandlingMethod;
	checkf(projClass, TEXT("No projectile to fire"));
}

bool UWeaponFiringStrategyBase::HasRecoil()
{
	return m_firingStrategyData.m_hasRecoil;
}

bool UWeaponFiringStrategyBase::InstantCooldown()
{
	return m_firingStrategyData.m_instantCooldown;
}

int UWeaponFiringStrategyBase::GetUseCountBeforeOnCooldown()
{
	checkf(m_firingStrategyData.m_useCountBeforeOnCooldown > 0, TEXT("Use count cannot be 0, have you forget to set it?"));

	int currentUseCount = m_firingStrategyData.m_currentTimesUsed;
	m_firingStrategyData.m_currentTimesUsed = (m_firingStrategyData.m_currentTimesUsed + 1) % m_firingStrategyData.m_useCountBeforeOnCooldown;
	return m_firingStrategyData.m_useCountBeforeOnCooldown - currentUseCount;
}

float UWeaponFiringStrategyBase::GetIntervalBeforeCooldown()
{
	return m_firingStrategyData.m_useIntervalBeforeCooldown;
}

float UWeaponFiringStrategyBase::GetStrategyFiringSpeedMult() const
{
	return m_firingSpeedMult;
}

bool UWeaponFiringStrategyBase::IsInUse()
{
	return m_isInUse;
}