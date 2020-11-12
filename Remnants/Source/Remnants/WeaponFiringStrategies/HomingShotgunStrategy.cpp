// Fill out your copyright notice in the Description page of Project Settings.

#include "HomingShotgunStrategy.h"
#include "../Core/Items/ShootergameProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UHomingShotgunStrategy::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
	int damage, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
	float range, float explosionRadiusMult, float moduleSizeMult, float elementProjectileSpeedMult,
	TSubclassOf<AShootergameProjectile> projClass, TSubclassOf<class AShootergameBeam> beamClass, TSubclassOf<UElementDamageType> elementDamageType,
	FName shooterType, int moduleType, USoundBase* elementSound)
{
	Super::FiringLogic(shooter, spawnLoc, spawnRot, world,
		damage, moduleCriticalChance, moduleCriticalDamageMult, elementLvL,
		range, explosionRadiusMult, moduleSizeMult, elementProjectileSpeedMult,
		projClass, beamClass, elementDamageType, shooterType, moduleType, elementSound);
}

void UHomingShotgunStrategy::EndFire()
{
	Super::EndFire();
}

void UHomingShotgunStrategy::SetHomingTargetActor(AActor* target, float magnitude)
{
	m_homingTargetActor = target;
	m_homingMagnitude = magnitude;
}

void UHomingShotgunStrategy::OnSpawnedProjectile(AShootergameProjectile* proj)
{
	if (!proj)
		return;
	if (!m_homingTargetActor)
		return;

	UProjectileMovementComponent* projMovement = proj->GetProjectileMovement();
	projMovement->HomingAccelerationMagnitude = m_homingMagnitude;
	projMovement->HomingTargetComponent = m_homingTargetActor->GetRootComponent();
	//projMovement->bIsHomingProjectile = true;
}


