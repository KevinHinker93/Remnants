// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBehaviourComponentBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UProjectileBehaviourComponentBase::UProjectileBehaviourComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UProjectileBehaviourComponentBase::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	m_damage = damage; 
	m_shooterType = shooterType;
	m_moduleType = moduleType;
	m_damageCauser = projectileSpawner;
	m_damageTypeClass = damageTypeClass;

	m_projectileMovement = projectileMovement;
	m_projectileRange = range;
	
	if (!m_projectile)
	{
		AActor* owner = GetOwner();
		m_projectile = Cast<AShootergameProjectile>(owner);
	}
}

void UProjectileBehaviourComponentBase::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	FVector spawnLoc = HitInfo.ImpactPoint;
	m_projectile->OnExplode(spawnLoc, true, m_particleExplosionSizeMult);
	if(DamagedActor->ActorHasTag("Destructible"))
		UGameplayStatics::ApplyPointDamage(DamagedActor, m_damage, HitFromDirection, HitInfo, EventInstigator, m_damageCauser, m_damageTypeClass);
}

float UProjectileBehaviourComponentBase::CalcLifeTimeFromProjectileRange(float range)
{
	float rangeInCentimeter = 100.0f * range;
	return range / (m_projectileMovement->InitialSpeed / 100);
}

float UProjectileBehaviourComponentBase::CalcTotalDamage(float initialDamage)
{
	return m_projectile->CalcTotalDamage(initialDamage);
}

bool UProjectileBehaviourComponentBase::CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors)
{
	FHitResult HitResult;
 
    /*The Start location of the sphere*/
	FVector StartLocation = target->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_projectile->GetActorLocation();

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActors(ignoredActors);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, queryParams);
	if(bHitSomething)
		GLog->Log(HitResult.GetActor()->GetName());
	return bHitSomething;
}

void UProjectileBehaviourComponentBase::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult,
		int elementLvL,	float explosionRadiusMult)
{
	FVector explosionLoc = m_projectile->GetActorLocation();
	m_projectile->OnExplode(explosionLoc, true, m_particleExplosionSizeMult);
}