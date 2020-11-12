// Fill out your copyright notice in the Description page of Project Settings.

#include "BouncingProjectileComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"

void UBouncingProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner,
	TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);
	m_currentBounceCount = 0;

	projectileMovement->bShouldBounce = true;
	projectileMovement->Bounciness = 1.0f;
	projectileMovement->Friction = 0.0f;

	FVector projScale = m_projectile->GetActorScale3D();
	m_projectile->SetActorScale3D(m_projectileSizeMult * projScale);
}

void UBouncingProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	FVector hitLoc = HitInfo.ImpactPoint;
	
	if (DamagedActor->ActorHasTag("Destructible") || DamagedActor->ActorHasTag("Loot"))
	{
		UGameplayStatics::ApplyPointDamage(DamagedActor, m_damage, HitFromDirection, HitInfo, EventInstigator, m_damageCauser, m_damageTypeClass);
		m_projectile->OnExplode(hitLoc, true, m_particleExplosionSizeMult);
		return;
	}

	if (m_currentBounceCount++ >= m_maxBounces)
		m_projectile->OnExplode(hitLoc, true, m_particleExplosionSizeMult);
	else
	{
		if (m_bounceSound != NULL)
			UGameplayStatics::PlaySoundAtLocation(this, m_bounceSound, m_projectile->GetActorLocation());
		m_projectile->SetLifeSpan(m_projectile->GetProjectileMaxLifeTime() * m_bounceLifetimeExtendMult);
		m_projectile->OnExplode(hitLoc, false, m_particleExplosionSizeMult);
	}
}

void UBouncingProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	float damageToDeal = CalcTotalDamage(m_damage);
	damageToDeal *= ((m_currentBounceCount + 1) * m_damageMultPerBounce);

	UGameplayStatics::ApplyPointDamage(DamagedActor, damageToDeal, HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}

void UBouncingProjectileComponent::Bounce(const FVector & hitLoc)
{
	
}


