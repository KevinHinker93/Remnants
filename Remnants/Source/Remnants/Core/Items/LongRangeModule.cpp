// Fill out your copyright notice in the Description page of Project Settings.

#include "LongRangeModule.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"

ULongRangeModule::ULongRangeModule()
{
	m_canFire = true;
}

void ULongRangeModule::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
	int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType)
{
	if (world != NULL)
	{
		if (m_world == nullptr)
			m_world = world;

		auto projClass = element->GetProjectile();
		auto beamClass = element->GetBeam();

		float moduleDamMult = GetDamageMult();
		float elementDamMult = element->GetElementalDamageMult();
		float totalDamageMult = elementDamMult * moduleDamMult;

		int totalDamage = damage * totalDamageMult * damageMult;
		float criticalChance = GetCriticalChance();
		float criticalDamageMult = GetCriticalMult();
		float elementRange = element->GetElementMinRange(int(m_moduleType));
		float range = 0;
		if (elementRange == 0)
			range = m_range;
		else
			range = elementRange;

		UWeaponFiringStrategyBase* elementFiringStrategy = element->GetElementWeaponFiringStrategy(int(m_moduleType)).GetDefaultObject();

		if (!m_firingStrat)
		{
			m_firingStrat = DuplicateObject(elementFiringStrategy, elementFiringStrategy->GetOuter());
			UE_LOG(LogTemp, Display, TEXT("Firing Strat initialized in module: %s"),*GetName());
		}
		else if (elementFiringStrategy->GetClass() != m_firingStrat->GetClass())
		{
			UE_LOG(LogTemp, Display, TEXT("Firing Strat has changed probably due to element change: %s to %s"),*m_firingStrat->GetName(), *elementFiringStrategy->GetName());
			m_firingStrat = DuplicateObject(elementFiringStrategy, elementFiringStrategy->GetOuter());
		}

		m_firingStrat->FiringLogic(shooter, spawnLoc, spawnRot, world, totalDamage, criticalChance, criticalDamageMult, element->GetElementLevel(), 
				range, m_explosionRadiusMult, m_sizeMult, element->GetElementalProjectileSpeedMult(), projClass, beamClass, element->GetDamageTypeClass(), shooterType, int(m_moduleType),
				element->GetElementSound());


		float totalFiringSpeedDiv = element->GetElementalFiringSpeedDiv() * GetTotalFiringSpeed();
		float weaponFiringSpeed = totalFiringSpeedDiv != 0 ? firingSpeed / totalFiringSpeedDiv : firingSpeed;
		float strategyFiringSpeedMult = m_firingStrat->GetStrategyFiringSpeedMult();
		m_totalFiringSpeed = weaponFiringSpeed * firingSpeedMult * strategyFiringSpeedMult;
		
		if (m_firingStrat->InstantCooldown())
		{
			int useCount = m_firingStrat->GetUseCountBeforeOnCooldown();
			if(useCount == 1)
				m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
			else if(useCount > 1)
				m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_firingStrat->GetIntervalBeforeCooldown(), false);
		}
		

	}
}

void ULongRangeModule::StopFiring(UWorld* world)
{
	if (m_world == nullptr)
		m_world = world;

	if (!m_firingStrat) return;

	m_firingStrat->EndFire();

	if (!m_firingStrat->InstantCooldown())
		m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
}

// Call function to check if module has a recoil or not
// when firing
bool ULongRangeModule::GetHasRecoil()
{
	return m_firingStrat->HasRecoil();
}

bool ULongRangeModule::IsInUse()
{
	if (m_firingStrat != NULL)
		return m_firingStrat->IsInUse();
	else
		return false;
}