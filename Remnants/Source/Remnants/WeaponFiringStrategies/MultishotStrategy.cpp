// Fill out your copyright notice in the Description page of Project Settings.

#include "MultishotStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"
#include "../AbleToShoot.h"

void UMultishotStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound)
{
	Super::FiringLogic(shooter, spawnLoc, spawnRot, world,
			damage, moduleCriticalChance, moduleCriticalDamageMult, elementLvL,
			range, explosionRadiusMult, moduleSizeMult, elementProjectileSpeedMult,
			projClass, beamClass, elementDamageType, shooterType, moduleType, elementSound);

	if (m_world != NULL)
	{
		m_multipleShotCount = m_maxMultipleShotCount;
		world->GetTimerManager().ClearTimer(m_multipleShotIntervalTimerHandle);

		//Binding the function with specific values
		m_timerDelegate.BindUFunction(this, FName("OnMultipleShot"), shooter, spawnLoc, spawnRot, world, damage, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, range,
				explosionRadiusMult, moduleSizeMult, elementProjectileSpeedMult, projClass, elementDamageType, shooterType, moduleType, elementSound);

		world->GetTimerManager().SetTimer(m_multipleShotIntervalTimerHandle, m_timerDelegate, m_multipleShotInterval, false);

		m_multipleShotCount--;
		AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
			proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
					explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);
		PlaySoundOnFire(spawnLoc, elementSound, proj->GetRootComponent());
	}

	m_isInUse = false;
}

void UMultishotStrategy::EndFire()
{

}

void UMultishotStrategy::OnMultipleShot(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
		float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
		TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<UElementDamageType> elementDamageType,
		FName shooterType, int moduleType, USoundBase* elementSound)
{
	AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
			proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
					explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);
	PlaySoundOnFire(spawnLoc, elementSound, proj->GetRootComponent());
	m_multipleShotCount--;
	if(m_multipleShotCount > 0)
		world->GetTimerManager().SetTimer(m_multipleShotIntervalTimerHandle, m_timerDelegate, m_multipleShotInterval, false);
	else
		world->GetTimerManager().ClearTimer(m_multipleShotIntervalTimerHandle);

}
