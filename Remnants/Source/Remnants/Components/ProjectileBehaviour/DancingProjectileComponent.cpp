// Fill out your copyright notice in the Description page of Project Settings.

#include "DancingProjectileComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UDancingProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	m_projectileMovement->bIsHomingProjectile = false;
	m_startedToDance = false;
	m_lifeTime = CalcLifeTimeFromProjectileRange(range);
	FVector newSize = FVector(m_projectileSizeMult, m_projectileSizeMult, m_projectileSizeMult);
	m_projectile->GetRootComponent()->SetWorldScale3D(newSize);
}

void UDancingProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UDancingProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
	
	bool canJump = m_jumpCount > 0;
	FName shooterType = m_projectile->GetShooterType();
	if (shooterType == "Enemy")
		canJump = false;
	if (canJump)
	{
		m_enemiesHit.Add(DamagedActor);
		FindNextTarget(DamagedActor);
	}
	m_jumpCount--;
	m_projectile->OnExplode(HitInfo.ImpactPoint, !canJump, m_particleExplosionSizeMult);
}

void UDancingProjectileComponent::EndCollision()
{

}

void UDancingProjectileComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}

void UDancingProjectileComponent::FindNextTarget(AActor* lastHitActor)
{
	/*TArray is the collection that contains all the HitResults*/
    TArray<FHitResult> HitResults;
 
    /*The Start location of the sphere*/
    FVector StartLocation = m_projectile->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_projectile->GetActorLocation();
    EndLocation.Z += m_jumpRadius;
 
    /*Search for static objects only*/
    ECollisionChannel ECC = ECollisionChannel::ECC_WorldStatic;
 
    /*Declare the Collision Shape, assign a Sphere shape and set it's radius*/
    FCollisionShape CollisionShape;
    CollisionShape.ShapeType = ECollisionShape::Sphere;
    CollisionShape.SetSphere(m_jumpRadius);
	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActors(m_enemiesHit);

    /*Perform the actual raycast. This method returns true if there is at least 1 hit.*/
    bool bHitSomething = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::FQuat(), ECC, CollisionShape);
 
	TSet<AActor*> enemiesInRangeSet;

    /*If the raycast hit a number of objects, iterate through them and print their name in the console*/
    if (bHitSomething)
    {
		TArray<AActor*> ignoredActors;
		ignoredActors.Add(m_damageCauser);
		ignoredActors.Add(m_projectile);

		// Get all enemies, there is always a second drone which prevents connections !!! hotfix
		TArray<AActor*> enemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAEnemyDrone::StaticClass(), enemies);

		// Add projectiles to ignore list
		ignoredActors += enemies;

		float minAbsDistance = m_jumpRadius + 100; // start outside range
		m_nextTarget = nullptr;
        for (auto It = HitResults.CreateIterator(); It; It++)
        {
			if ((*It).Actor->ActorHasTag("Enemy") && (*It).Actor != lastHitActor)
			{
				GLog->Log((*It).Actor->GetName());
				float absDistance = FMath::Abs(FVector::Dist((*It).Actor->GetActorLocation(), m_projectile->GetActorLocation()));
				if (absDistance < minAbsDistance)
				{
					AActor* newTarget = (*It).Actor.Get();
					if (!CheckTargetIsBlocked(newTarget, ignoredActors))
						enemiesInRangeSet.Add(newTarget);
				}
			}
        }
    }
 
    /*In order to draw the sphere of the first image, I will use the DrawDebugSphere function which resides in the DrawDebugHelpers.h
    This function needs the center of the sphere which in this case is provided by the following equation*/
    FVector CenterOfSphere = ((EndLocation - StartLocation) / 2) + StartLocation;
 
    /*Draw the sphere in the viewport*/
    // UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CenterOfSphere, CollisionShape.GetSphereRadius(), 100, FColor::Green, true);

	// Sorts set so nearest actor is on index 0
	enemiesInRangeSet.Sort([this](AActor& A, AActor& B)
	{
		float absDistanceA = FMath::Abs(FVector::Dist(A.GetActorLocation(), m_projectile->GetActorLocation()));
		float absDistanceB = FMath::Abs(FVector::Dist(B.GetActorLocation(), m_projectile->GetActorLocation()));

		return absDistanceA < absDistanceB; // sort by reverse-alphabetical order
	});
	// converts set to array
	TArray<AActor*> sortedEnemyArray = enemiesInRangeSet.Array();
	if(sortedEnemyArray.Num() > 0)
		m_nextTarget = sortedEnemyArray[0];

	float minAbsDistance = m_jumpRadius + 100; // start outside range

	if (m_nextTarget)
	{
		if (!m_startedToDance)
		{
			m_startedToDance = true;
			m_projectileMovement->InitialSpeed /= m_dancingSpeedDivisor;
			m_projectileMovement->MaxSpeed /= m_dancingSpeedDivisor;
		}
		m_projectileMovement->bIsHomingProjectile = true;
		m_projectileMovement->HomingTargetComponent = m_nextTarget->GetRootComponent();
		m_projectileMovement->HomingAccelerationMagnitude = m_jumpMagnitude;

		// Reset lifetime of projectile if it homes to target
		m_projectileMovement->GetOwner()->SetLifeSpan(m_lifeTime);
	}
	else
		m_projectile->Destroy();
	
}

bool UDancingProjectileComponent::CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors)
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

