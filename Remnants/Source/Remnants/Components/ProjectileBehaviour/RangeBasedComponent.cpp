// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeBasedComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"

void URangeBasedComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	float timeToLive = m_projectile->GetLifeSpan();
	float maxLifeTime = m_projectile->GetProjectileMaxLifeTime();
	float timeLived = maxLifeTime - timeToLive;

	float damageToDeal = CalcTotalDamage(m_damage);
	if (maxLifeTime > 0.0f)
	{
		float livedPerc = (timeLived / maxLifeTime);
		livedPerc = livedPerc * livedPerc;
		float damageMult =  livedPerc * m_rangeDamageMult;
		damageToDeal *= damageMult;
	}

	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	for(int i = 0; i < m_damageRate; i++)
		UGameplayStatics::ApplyPointDamage(DamagedActor, damageToDeal, HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}

void URangeBasedComponent::EndCollision()
{

}


