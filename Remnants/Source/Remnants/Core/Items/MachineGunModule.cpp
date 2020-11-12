// Fill out your copyright notice in the Description page of Project Settings.

#include "MachineGunModule.h"
#include "ElementDamageType.h"
#include "WeaponFiringStrategies/WeaponFiringStrategyBase.h"
#include "WeaponFiringStrategies/SniperStrategy.h"
#include "WeaponFiringStrategies/ThrowerStrategy.h"


UMachineGunModule::UMachineGunModule()
{
	m_canFire = true;
	m_moduleType = -1;
}

void UMachineGunModule::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType)
{
	if (world != NULL)
	{
		if (m_world == nullptr)
			m_world = world;
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		auto moduleElement = element;
		auto projClass = element->GetProjectile();
			
		float moduleDamMult = GetDamageMult();
		float elementDamMult = element->GetElementalDamageMult();
		float totalDamageMult = elementDamMult * moduleDamMult;

		int totalDamage = damage * totalDamageMult * damageMult;
		float criticalChance = GetCriticalChance();
		float criticalDamageMult = GetCriticalMult();
		if (!projClass)
		{
			UE_LOG(LogTemp, Error, TEXT("MachineGun Module has no proj to fire"));
			return;
		}

		if (m_moduleType != -1)
		{
			auto beamClass = element->GetBeam();
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
				UE_LOG(LogTemp, Display, TEXT("Firing Strat initialized in module: %s"), *GetName());
			}
			else if (elementFiringStrategy->GetClass() != m_firingStrat->GetClass())
			{
				UE_LOG(LogTemp, Display, TEXT("Firing Strat has changed probably due to element change: %s to %s"), *m_firingStrat->GetName(), *elementFiringStrategy->GetName());
				m_firingStrat = DuplicateObject(elementFiringStrategy, elementFiringStrategy->GetOuter());
			}

			m_firingStrat->FiringLogic(shooter, spawnLoc, spawnRot, world, totalDamage, criticalChance, criticalDamageMult, element->GetElementLevel(),
				range, m_explosionRadiusMult, m_sizeMult, element->GetElementalProjectileSpeedMult(), projClass, beamClass, element->GetDamageTypeClass(), shooterType, int(m_moduleType),
				element->GetElementSound());
		}
		else
		{
			// spawn the projectile at the muzzle
			AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, ActorSpawnParams);
			proj->InitProjectile(totalDamage, criticalChance, criticalDamageMult, element->GetElementalProjectileSpeedMult(), m_sizeMult,
				m_explosionRadiusMult, m_range, shooterType, m_moduleType, shooter, element->GetDamageTypeClass());

			if (m_FireSound)
				PlaySoundOnFire(spawnLoc, element->GetElementSound(), proj->GetRootComponent());
		}


		float totalFiringSpeedDiv = element->GetElementalFiringSpeedDiv() * GetTotalFiringSpeed();
		float weaponFiringSpeed = totalFiringSpeedDiv != 0 ? firingSpeed / totalFiringSpeedDiv : firingSpeed;
		float totalFiringSpeed = weaponFiringSpeed * firingSpeedMult;
		m_totalFiringSpeed = totalFiringSpeed;
		m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
	}
}

void UMachineGunModule::StopFiring(UWorld* world)
{
	if (m_world == nullptr)
		m_world = world;

	m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
}

void UMachineGunModule::SetModuleType(int moduleType)
{
	m_moduleType = moduleType;
}