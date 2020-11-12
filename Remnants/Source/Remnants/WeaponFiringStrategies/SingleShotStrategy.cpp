// Fill out your copyright notice in the Description page of Project Settings.

#include "SingleShotStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"

void USingleShotStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
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
		// spawn the projectile at the muzzle
		AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
		proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
			explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);

		PlaySoundOnFire(spawnLoc, elementSound, proj->GetRootComponent());
	}

	m_isInUse = false;
}

void USingleShotStrategy::EndFire()
{

}
