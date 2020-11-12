// Fill out your copyright notice in the Description page of Project Settings.

#include "ShotgunModule.h"

UShotgunModule::UShotgunModule()
{
	m_rotOffset = 12;

	m_projectileAtOrigin = true;
	m_numCircles = 1;
	m_numProjectiles = 8;

	m_canFire = true;
}

void UShotgunModule::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
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
			UE_LOG(LogTemp, Error, TEXT("Shotgun Module has no proj to fire"));
			return;
		}

		float rotOffsetX = m_rotOffset * 1.5f;
		float rotOffsetY = 0;
		float step = m_rotOffset / 2;

		float angle = 360 / m_numProjectiles;
		float mystep = 0;
		FMath::RandInit(int(spawnLoc.X) * world->RealTimeSeconds);

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
				AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, rotOffset, ActorSpawnParams);
				proj->InitProjectile(totalDamage, criticalChance, criticalDamageMult, element->GetElementalProjectileSpeedMult(), m_sizeMult,
						m_explosionRadiusMult, m_range, shooterType, -1, shooter, element->GetDamageTypeClass());

				mystep += angle * PI / 180;
			}
		}

		if (m_projectileAtOrigin)
		{
			AShootergameProjectile* projOrigin = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, ActorSpawnParams);
			projOrigin->InitProjectile(totalDamage, criticalChance, criticalDamageMult, element->GetElementalProjectileSpeedMult(), m_sizeMult,
					m_explosionRadiusMult, m_range, shooterType, -1, shooter, element->GetDamageTypeClass());

			if (m_FireSound)
				PlaySoundOnFire(spawnLoc, element->GetElementSound(), projOrigin->GetRootComponent());
		}

		float totalFiringSpeedDiv = element->GetElementalFiringSpeedDiv() * GetTotalFiringSpeed();
		float weaponFiringSpeed = totalFiringSpeedDiv != 0 ? firingSpeed / totalFiringSpeedDiv : firingSpeed;
		float totalFiringSpeed = weaponFiringSpeed * firingSpeedMult;
		m_totalFiringSpeed = totalFiringSpeed;
		m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
	}
}

void UShotgunModule::StopFiring(UWorld* world)
{
	if (m_world == nullptr)
		m_world = world;
	m_world->GetTimerManager().SetTimer(m_cooldownTimerHandle, this, &UAModule::EnableUse, m_totalFiringSpeed, false);
}

