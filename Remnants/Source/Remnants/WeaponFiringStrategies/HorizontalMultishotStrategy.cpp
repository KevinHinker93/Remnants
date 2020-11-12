// Fill out your copyright notice in the Description page of Project Settings.

#include "HorizontalMultishotStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"

void UHorizontalMultishotStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
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
		int projectileCount = m_projectileCountsAtElementLvl[elementLvL];
		AShootergameProjectile* proj = nullptr;
		if (projectileCount <= 1 || projectileCount % 2 == 0)
		{
			// spawn the projectile at the muzzle
			proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
			proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
					explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);
		}
		else
		{
			for (int i = -projectileCount/2; i <= projectileCount/2; i++)
			{
				FRotator rotOffset = spawnRot;
				rotOffset.Yaw += m_rotOffset * i;
				// spawn the projectile at the muzzle
				proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
				proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
					explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);
			}
		}
		if(proj)
			PlaySoundOnFire(spawnLoc, elementSound, proj->GetRootComponent());
	}

	m_isInUse = false;
}

void UHorizontalMultishotStrategy::EndFire()
{

}
