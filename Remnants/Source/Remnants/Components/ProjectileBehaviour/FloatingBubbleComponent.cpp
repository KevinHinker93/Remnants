// Fill out your copyright notice in the Description page of Project Settings.

#include "FloatingBubbleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"

void UFloatingBubbleComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	float startToFloatTime = CalcLifeTimeFromProjectileRange(m_startToFloatDist); 
	GetWorld()->GetTimerManager().SetTimer(m_speedTimerHandle, this, &UFloatingBubbleComponent::OnStartFloating, startToFloatTime, false);
	m_projectileRange -= m_startToFloatDist;
	if (m_randomLifetimeMin < m_randomLifetimeMax &&(m_randomLifetimeMin != 0 || m_randomLifetimeMax != 0))
	{
		int randSeed = FMath::Rand();
		FMath::RandInit(randSeed * damage);
		float randomLifetime = FMath::RandRange(m_randomLifetimeMin, m_randomLifetimeMax);
		m_projectileRange *= randomLifetime;
	}
		
	m_currentBubbleSize = 0.4f;
	if (m_sizeMult > 0)
	{
		FVector newSize = FVector(m_currentBubbleSize, m_currentBubbleSize, m_currentBubbleSize);
		m_projectile->GetRootComponent()->SetWorldScale3D(newSize);
		if (m_interpolationFactor > 0)
			InterpolateBubbleSize();
		else
		{
			newSize = FVector(m_sizeMult, m_sizeMult, m_sizeMult);
			m_projectile->GetRootComponent()->SetWorldScale3D(newSize);
		}
	}

}

void UFloatingBubbleComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UFloatingBubbleComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, !m_isMultihit, m_sizeMult);
	float damage = m_damage * (m_currentBubbleSize * m_damageMult);
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
	
	if (m_isMultihit)
	{
		//Binding the function with specific values
		if (!GetWorld()->GetTimerManager().IsTimerActive(m_multipleHitIntervalTimerHandle) && !GetOwner()->IsPendingKill())
			m_timerDelegate.BindUFunction(this, FName("OnMultipleHit"), HitFromDirection, HitInfo, NULL);
		// Saving these two as member variable, since the delegate bind function does not seem to correctly bind it, damageCauser will be damageTypeClass and DamageTypeC = null

		GetWorld()->GetTimerManager().SetTimer(m_multipleHitIntervalTimerHandle, m_timerDelegate, m_hitInterval, true);
	}
}

void UFloatingBubbleComponent::EndCollision()
{

}

void UFloatingBubbleComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	FVector explosionLoc = m_projectile->GetActorLocation();
	m_projectile->OnExplode(explosionLoc, true, m_particleExplosionSizeMult * m_currentBubbleSize);
}

void UFloatingBubbleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_interpolate)
	{
		float nextVal = m_sizeDiff * m_interpolationFactor * DeltaTime;
		m_currentBubbleSize += nextVal;
		m_absSizeDiff -= FMath::Abs(nextVal);
		m_projectile->GetRootComponent()->SetWorldScale3D(FVector(m_currentBubbleSize, m_currentBubbleSize, m_currentBubbleSize));
		if (m_absSizeDiff < 0)
		{
			m_interpolate = false;
			float floatingLifeTime = CalcLifeTimeFromProjectileRange(m_projectileRange);
			m_projectileMovement->GetOwner()->SetLifeSpan(floatingLifeTime);
		}
	}
}

void UFloatingBubbleComponent::OnStartFloating()
{
	GetWorld()->GetTimerManager().ClearTimer(m_speedTimerHandle);
	if (m_floatingSpeed > 0)
	{
		m_projectileMovement->InitialSpeed = m_floatingSpeed;
		m_projectileMovement->MaxSpeed = m_floatingSpeed;
	}
	else
		m_projectileMovement->Deactivate();
	
	m_projectileMovement->ProjectileGravityScale = m_floatingGravityScale;
	float floatingLifeTime = CalcLifeTimeFromProjectileRange(m_projectileRange);
	m_projectileMovement->GetOwner()->SetLifeSpan(floatingLifeTime);
}

void UFloatingBubbleComponent::InterpolateBubbleSize()
{
	//m_currentBubbleSize = m_projectile->GetProjectileSize();
	m_desiredBubbleSize = m_sizeMult;
	m_sizeDiff = m_desiredBubbleSize - m_currentBubbleSize;
	m_absSizeDiff = FMath::Abs(m_sizeDiff);
	//UE_LOG(LogTemp, Error, TEXT("Bubble Size: %f // desired Bubble size: %f"), m_currentBubbleSize, m_desiredBubbleSize, m_sizeDiff);
	m_projectileMovement->GetOwner()->SetLifeSpan(0);
	m_interpolate = true;
}

void UFloatingBubbleComponent::OnMultipleHit(FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	if (!m_projectile)
		return;

	TArray<AActor*> overlappingActors;
	m_projectile->GetOverlappingActors(overlappingActors);

	if (overlappingActors.Num() > 0)
	{
		FName shooterType = m_projectile->GetShooterType();
		if (shooterType == "Player")
			shooterType = "Enemy";
		else if (shooterType == "Enemy")
			shooterType = "Player";

		for (int i = 0; i < overlappingActors.Num(); i++)
		{
			AActor* hitActor = overlappingActors[i];
			if (!hitActor || hitActor->IsPendingKill())
				continue;
			if ((hitActor->ActorHasTag(shooterType) || hitActor->ActorHasTag("Destructible") || hitActor->ActorHasTag("Loot")))
			{
				m_projectile->OnExplode(HitInfo.ImpactPoint, false, m_sizeMult);
				float damage = m_damage * (m_currentBubbleSize * m_damageMult);
				UGameplayStatics::ApplyPointDamage(hitActor, CalcTotalDamage(damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
			}
		}
	}
}