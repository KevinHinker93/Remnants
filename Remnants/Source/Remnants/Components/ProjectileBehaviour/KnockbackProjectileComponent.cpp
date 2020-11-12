// Fill out your copyright notice in the Description page of Project Settings.

#include "KnockbackProjectileComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UKnockbackProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);
}

void UKnockbackProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UKnockbackProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, false, m_particleExplosionSizeMult);
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);

	FVector direction = m_projectile->GetActorForwardVector();
	direction.Normalize();

	AAEnemyDrone* enemy = Cast<AAEnemyDrone>(DamagedActor);
	if (!enemy)
	{
		m_projectile->Destroy();
		return;
	}
	float enemyMass = enemy->PushPullResistance();
	UE_LOG(LogTemp, Verbose, TEXT("Actor mass: %f"), enemyMass);
	checkf(enemyMass > 0, TEXT("Zero mass / pushPullResistance not allowed, have you forgot to set it?"));
	float desiredKnockbackStrenght = m_knockbackStrenght / enemyMass;

	if (desiredKnockbackStrenght < m_pushThreshold)
	{
		m_projectile->Destroy();
		return;
	}
	FVector knockbackEndPoint = (direction * desiredKnockbackStrenght) + DamagedActor->GetActorLocation();

	KnockbackEnemy.Broadcast(DamagedActor->GetActorLocation(), knockbackEndPoint, DamagedActor);

	// Deactivate projectile so it can handle knockback before beeing destroyed
	m_projectile->GetRootComponent()->SetVisibility(false, true);
	m_projectile->GetCollisionComp()->Deactivate();
	m_projectileMovement->Deactivate();
	m_projectile->SetLifeSpan(0);
}

void UKnockbackProjectileComponent::EndCollision()
{

}

void UKnockbackProjectileComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}



