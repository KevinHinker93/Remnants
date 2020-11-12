// Fill out your copyright notice in the Description page of Project Settings.

#include "SpreadingShotgunComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "../../WeaponFiringStrategies/WeaponFiringStrategyBase.h"

USpreadingShotgunComponent::USpreadingShotgunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_spreadShotFiringStrategies.SetNum(3);
	m_hitObject = false;
}

void USpreadingShotgunComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);
}

void USpreadingShotgunComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
	m_hitObject = true;
}

void USpreadingShotgunComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	if (DamagedActor->IsPendingKill())
		return;
	m_hitObject = true;
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);
}

void USpreadingShotgunComponent::EndCollision()
{

}

void USpreadingShotgunComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	if (m_hitObject)
		return;
	FRotator spawnRot = m_projectile->GetActorRotation();
	FVector spawnLoc = m_projectile->GetActorLocation();

	// 0 should be replaced by element level
	UWeaponFiringStrategyBase* firingStrategy = m_spreadShotFiringStrategies[0].GetDefaultObject();
	if (!m_spreadShotFiringStrategy)
		m_spreadShotFiringStrategy = DuplicateObject(firingStrategy, firingStrategy->GetOuter());

	m_spreadShotFiringStrategy->FiringLogic(shooter, spawnLoc, spawnRot, m_projectile->GetWorld(), m_damage, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, 
				m_range, explosionRadiusMult, m_projectile->GetProjectileSize() * m_sizeMult, 1, m_projectile->GetClass(), NULL, m_damageTypeClass, m_shooterType, -1, nullptr);
	OnSpread.Broadcast();
}



