// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultProjectileBehaviourComp.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"

void UDefaultProjectileBehaviourComp::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

}

void UDefaultProjectileBehaviourComp::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UDefaultProjectileBehaviourComp::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}

void UDefaultProjectileBehaviourComp::EndCollision()
{

}

void UDefaultProjectileBehaviourComp::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}


