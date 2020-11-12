// Fill out your copyright notice in the Description page of Project Settings.

#include "PiercingShotComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"

void UPiercingShotComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);
}

void UPiercingShotComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UPiercingShotComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	bool keepProjectileAlive = m_piercingCount > 0;
	UE_LOG(LogTemp, Error, TEXT("Piercing Count: %d"), m_piercingCount);
	m_projectile->OnExplode(HitInfo.ImpactPoint, !keepProjectileAlive, m_particleExplosionSizeMult);

	m_piercingCount--;
	if (m_multipleHits > 0)
	{
		m_hitActors.Add(DamagedActor);

		//Binding the function with specific values
		if(!GetWorld()->GetTimerManager().IsTimerActive(m_multipleHitIntervalTimerHandle) && !GetOwner()->IsPendingKill())
			m_timerDelegate.BindUFunction(this, FName("OnMultipleHit"), HitFromDirection, HitInfo, NULL);
		// Saving these two as member variable, since the delegate bind function does not seem to correctly bind it, damageCauser will be damageTypeClass and DamageTypeC = null

		GetWorld()->GetTimerManager().SetTimer(m_multipleHitIntervalTimerHandle, m_timerDelegate, m_multipleHitInterval, true);
	}
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}

void UPiercingShotComponent::EndCollision()
{
	if(m_hitActors.Num() > 0)
		m_hitActors.Empty();
	if (GetWorld()->GetTimerManager().IsTimerActive(m_multipleHitIntervalTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(m_multipleHitIntervalTimerHandle);
}

void UPiercingShotComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}

void UPiercingShotComponent::OnMultipleHit(FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	if (m_multipleHits > 0)
	{
		for (int i = 0; i < m_hitActors.Num(); ++i)
		{
			auto actor = m_hitActors[i];
			if (actor)
				UGameplayStatics::ApplyPointDamage(actor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
			m_multipleHits--;
			m_projectile->OnExplode(HitInfo.ImpactPoint, false, m_particleExplosionSizeMult);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(m_multipleHitIntervalTimerHandle);
		m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);
	}
		
}
