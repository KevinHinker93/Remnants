// Fill out your copyright notice in the Description page of Project Settings.

#include "LifeLeechShotComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../HealthComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"

void ULifeLeechShotComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	float dealtDamage = UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
	if (m_damageCauser)
	{
		AActor* projectileSpawner = m_projectile->GetProjectileSpawner();
		UHealthComponent* healthComp = Cast<UHealthComponent>(m_damageCauser->GetComponentByClass(UHealthComponent::StaticClass()));
		float leechDamage = dealtDamage * m_leechPercentage;
		if(healthComp)
			healthComp->UpdateHealth(leechDamage);
	}
}

void ULifeLeechShotComponent::EndCollision()
{
	Super::EndCollision();
}