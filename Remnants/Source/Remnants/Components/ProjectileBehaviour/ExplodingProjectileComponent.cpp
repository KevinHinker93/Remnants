// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplodingProjectileComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"

void UExplodingProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	m_projectileMovement->ProjectileGravityScale = m_gravityInfluence;
	m_projectileMovement->InitialSpeed *= m_projectileSpeedMult;
	m_projectileMovement->MaxSpeed *= m_projectileSpeedMult;
	if (m_explodingTime > 0)
		m_projectile->SetLifeSpan(0);

	m_projectile->GetRootComponent()->SetWorldScale3D(FVector(m_sizeMult, m_sizeMult, m_sizeMult));
}

void UExplodingProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	PrepareExplosion(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UExplodingProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	PrepareExplosion(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UExplodingProjectileComponent::EndCollision()
{

}

void UExplodingProjectileComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	FVector HitFromDirection;
	FHitResult HitInfo;
	PrepareExplosion(nullptr, HitFromDirection, HitInfo, shooter->GetInstigatorController());
}

void UExplodingProjectileComponent::Explosion(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	OnExplode.Broadcast();
	ApplyRadialDamage(damagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UExplodingProjectileComponent::OnAttachedEnemyDestroyed(AActor* destroyedActor)
{
	Explosion(destroyedActor, FVector::ZeroVector, FHitResult(), nullptr);
}

void UExplodingProjectileComponent::ApplyRadialDamage(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	// Logic here
	m_projectile->OnExplode(m_projectile->GetActorLocation(), true, m_particleExplosionSizeMult * m_explosionRadius);
	m_firstHitActor = damagedActor;
	TArray<AActor*> actorsToDamage = GetActorsInRange();
	// Store already damaged enemies to avoid multiple damage
	TArray<AActor*> damagedActors;
	for (auto actor : actorsToDamage)
	{
		// The actor was already damaged ignore it
		if (damagedActors.Contains(actor))
			return;
		UGameplayStatics::ApplyPointDamage(actor, CalcTotalDamage(m_damage * m_damageMult), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
		damagedActors.Add(actor);
	}
	
}

TArray<AActor*> UExplodingProjectileComponent::GetActorsInRange()
{
	/*TArray is the collection that contains all the HitResults*/
    TArray<FHitResult> HitResults;
 
    /*The Start location of the sphere*/
    FVector StartLocation = m_projectile->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_projectile->GetActorLocation();
    EndLocation.Z += m_explosionRadius;
 
    ECollisionChannel ECC = ECollisionChannel::ECC_MAX;
 
    /*Declare the Collision Shape, assign a Sphere shape and set it's radius*/
    FCollisionShape CollisionShape;
    CollisionShape.ShapeType = ECollisionShape::Sphere;
    CollisionShape.SetSphere(m_explosionRadius);
	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(m_projectile);

	FCollisionResponseParams test;
	test = ECollisionResponse::ECR_Block;

    /*Perform the actual raycast. This method returns true if there is at least 1 hit.*/
    bool bHitSomething = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::FQuat(), ECC, CollisionShape, queryParams,test);
	TArray<AActor*> actorsInRange;
    /*If the raycast hit a number of objects, iterate through them and print their name in the console*/
    if (bHitSomething)
    {
		FName shooterType = m_projectile->GetShooterType();
		if (shooterType == "Player")
			shooterType = "Enemy";
		else if (shooterType == "Enemy")
			shooterType = "Player";

        for (auto It = HitResults.CreateIterator(); It; It++)
        {
			if (!(*It).Actor.IsValid())
				continue;
			AActor* hitActor = (*It).Actor.Get();
			if (!hitActor || hitActor->IsPendingKill())
				continue;
			if ((hitActor->ActorHasTag(shooterType) || hitActor->ActorHasTag("Destructible") || hitActor->ActorHasTag("Loot")) && !CheckTargetIsBlocked(hitActor, hitActor))
			{
				GLog->Log(hitActor->GetName());
				actorsInRange.AddUnique(hitActor);
			}
        }
    }
 
    /*In order to draw the sphere of the first image, I will use the DrawDebugSphere function which resides in the DrawDebugHelpers.h
    This function needs the center of the sphere which in this case is provided by the following equation*/
    FVector CenterOfSphere = ((EndLocation - StartLocation) / 2) + StartLocation;
 
    /*Draw the sphere in the viewport*/
    // UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CenterOfSphere, CollisionShape.GetSphereRadius(), 100, FColor::Green, true);

	return actorsInRange;
}

bool UExplodingProjectileComponent::CheckTargetIsBlocked(AActor* target, AActor* lastHitActor)
{
    FHitResult HitResult;
 
    /*The Start location of the sphere*/
	FVector StartLocation = target->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_projectile->GetActorLocation();

	TArray<AActor*> ignoredActors;
	ignoredActors.Add(target);
	if(m_firstHitActor)
		ignoredActors.Add(m_firstHitActor);
	ignoredActors.Add(m_projectile);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActors(ignoredActors);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, queryParams);
	if(bHitSomething)
		GLog->Log(HitResult.GetActor()->GetName());
	return bHitSomething;
}

void UExplodingProjectileComponent::PrepareExplosion(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	m_projectileMovement->ProjectileGravityScale = 0.0f;
	m_projectileMovement->Deactivate();
	if (m_explodingTime > 0)
	{
		m_timerDelegate.BindUFunction(this, FName("Explosion"), DamagedActor, HitFromDirection, HitInfo, NULL);
		if (DamagedActor && DamagedActor->Tags.Num() > 0)
		{
			if (DamagedActor->ActorHasTag("Enemy"))
			{
				m_projectile->AttachToComponent(DamagedActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				USceneComponent* attachParent = m_projectile->GetRootComponent()->GetAttachParent();
				if (!attachParent)
					GetOwner()->Destroy();
				AActor* attachedToActor = attachParent->GetOwner();
				if(attachedToActor)
					attachedToActor->OnDestroyed.AddDynamic(this, &UExplodingProjectileComponent::OnAttachedEnemyDestroyed);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(m_explosionTimerHandle, m_timerDelegate, m_explodingTime, false);
	}
	else
		Explosion(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}