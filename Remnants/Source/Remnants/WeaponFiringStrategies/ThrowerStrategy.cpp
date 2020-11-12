// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowerStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"
#include "../Core/Items/ShootergameBeam.h"
#include "AbleToShoot.h"

class AShootergameBeam;

void UThrowerStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
			int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
			TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
			FName shooterType, int moduleType, USoundBase* elementSound)
{
	Super::FiringLogic(shooter, spawnLoc, spawnRot, world,
		damage, moduleCriticalChance, moduleCriticalDamageMult, elementLvL,
		range, explosionRadiusMult, moduleSizeMult, elementProjectileSpeedMult,
		projClass, beamClass, elementDamageType, shooterType, moduleType, elementSound);

	m_beam = world->SpawnActor<AShootergameBeam>(beamClass, spawnLoc, spawnRot, m_projectileSpawnParams);
	m_beam->InitBeam(damage, shooterType, moduleType, shooter, elementDamageType);
	m_shooter = shooter;
	auto shootingActor = Cast<IAbleToShoot>(m_shooter);
	if (shootingActor)
	{
		m_beam->AttachToComponent(shootingActor->GetMuzzelLoc(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	m_isInUse = true;
}

void UThrowerStrategy::EndFire()
{
	auto shootingActor = Cast<IAbleToShoot>(m_shooter);
	if (shootingActor)
	{
		m_beam->DetachAllSceneComponents(shootingActor->GetMuzzelLoc(), FDetachmentTransformRules::KeepWorldTransform);
	}
	m_beam->OnEndBeam();
	//m_beam->Kill();
	m_isInUse = false;
}


