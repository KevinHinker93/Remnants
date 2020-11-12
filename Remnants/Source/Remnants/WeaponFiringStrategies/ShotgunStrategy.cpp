// Fill out your copyright notice in the Description page of Project Settings.

#include "ShotgunStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"

void UShotgunStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
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
		float rotOffsetX = m_rotOffset * 1.5f;
		float rotOffsetY = 0;
		float step = m_rotOffset / 2;

		float angle = 360 / m_numProjectiles;
		float mystep = 0;
		FMath::RandInit(int(spawnLoc.X) * world->RealTimeSeconds);
		AShootergameProjectile* projOrigin = nullptr;

		for (int circle = 1; circle <= m_numCircles; circle++)
		{
			for (int numProj = 0, i = 0, j = 2; numProj < m_numProjectiles; numProj++, i++, j++)
			{
				rotOffsetY = m_rotOffset * FMath::Cos(mystep);
				rotOffsetX = m_rotOffset * FMath::Sin(mystep);

				FRotator rotOffset = spawnRot;
				float randOffsetX = FMath::RandRange(-m_randOffset, m_randOffset);
				float randOffsetY = FMath::RandRange(-m_randOffset, m_randOffset);
				//UE_LOG(LogTemp, Error, TEXT("RandX:: %f // RandY:: %f"), randOffsetX, randOffsetY);
				rotOffset.Pitch += (rotOffsetX * circle) + randOffsetX;
				rotOffset.Yaw += (rotOffsetY * circle) + randOffsetY;
				AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, rotOffset, m_projectileSpawnParams);
				proj->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
						explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);

				OnSpawnedProjectile(proj);

				if (!projOrigin)
					projOrigin = proj;

				mystep += angle * PI / 180;
			}
		}

		if (m_projectileAtOrigin)
		{
			projOrigin = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, m_projectileSpawnParams);
			projOrigin->InitProjectile(damage, moduleCriticalChance, moduleCriticalDamageMult, elementProjectileSpeedMult, moduleSizeMult,
				explosionRadiusMult, range, shooterType, moduleType, shooter, elementDamageType);
			OnSpawnedProjectile(projOrigin);
		}

		if(projOrigin)
			PlaySoundOnFire(spawnLoc, elementSound, projOrigin->GetRootComponent());
	}

	m_isInUse = false;
}

void UShotgunStrategy::EndFire()
{

}

void UShotgunStrategy::SetRotOffset(float offset)
{
	m_rotOffset = offset;
}

void UShotgunStrategy::SetNumProjectiles(int numProjectiles)
{
	m_numProjectiles = numProjectiles;
}

void UShotgunStrategy::OnSpawnedProjectile(AShootergameProjectile* proj)
{

}