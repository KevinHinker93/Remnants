// Fill out your copyright notice in the Description page of Project Settings.

#include "ConnectingProjectileComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "../../Core/Items/ShootergameBeam.h"
#include "../../ElementDamageType.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "../../EventSystemHandlerSingleton.h"

void UConnectingProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	m_connectionLeft = m_maxConnectionCount;

	bool checkSingleton = false;
	m_eventSystem = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	m_eventSystem->OnProjectileConnectionRequest.AddDynamic(this, &UConnectingProjectileComponent::OnConnectRequest);
	m_eventSystem->OnProjectileConnectionReply.AddDynamic(this, &UConnectingProjectileComponent::OnConnectReply);
	// Event if this component/object is a connecter
	m_eventSystem->OnNotifyProjectileConnecter.AddDynamic(this, &UConnectingProjectileComponent::OnNotifyProjectileConnecter);
	// Event if this component/object is connected to the given actor
	m_eventSystem->OnNotifyConnectedProjectile.AddDynamic(this, &UConnectingProjectileComponent::OnNotifyConnectedProjectile);
}

void UConnectingProjectileComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{

}

void UConnectingProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UConnectingProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	if(!m_connectedActor)
		m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}

void UConnectingProjectileComponent::EndCollision()
{

}

void UConnectingProjectileComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	if (m_eventSystem)
	{
		m_eventSystem->OnProjectileConnectionRequest.RemoveDynamic(this, &UConnectingProjectileComponent::OnConnectRequest);
		m_eventSystem->OnProjectileConnectionReply.RemoveDynamic(this, &UConnectingProjectileComponent::OnConnectReply);
		m_eventSystem->OnNotifyProjectileConnecter.RemoveDynamic(this, &UConnectingProjectileComponent::OnNotifyProjectileConnecter);
		m_eventSystem->OnNotifyConnectedProjectile.RemoveDynamic(this, &UConnectingProjectileComponent::OnNotifyConnectedProjectile);
	}
	if (m_beam)
		m_beam->OnEndBeam();
	m_projectile->OnExplode(m_projectile->GetActorLocation(), false, m_particleExplosionSizeMult);
}

void UConnectingProjectileComponent::OnStartFloating()
{
	Super::OnStartFloating();
	m_projectile->SetLifeSpan(m_connectedTime);
	
	// --------- Get all actors with connecting tag in range
	TArray<AActor*> actorsInRange = GetActorsInRange();
	if (actorsInRange.Num() <= 0)
		return;

	// --------- Fetch nearest actor
	AShootergameProjectile* nearestProjectileToConnect = Cast<AShootergameProjectile>(actorsInRange[0]);

	if (nearestProjectileToConnect)
	{
		m_connectedActor = nearestProjectileToConnect;
		GLog->Log(m_connectedActor->GetName());

		FVector spawnLoc = m_projectile->GetActorLocation();
		FVector dist = m_connectedActor->GetActorLocation() - m_projectile->GetActorLocation();

		// ---------- Ask nearest actor if this object can connect to it
		m_eventSystem->OnProjectileConnectionRequest.Broadcast(GetOwner(), m_connectedActor, dist.Size());
	}
}

TArray<AActor*> UConnectingProjectileComponent::GetActorsInRange()
{
	/*TArray is the collection that contains all the HitResults*/
    TArray<FHitResult> HitResults;
 
    /*The Start location of the sphere*/
    FVector StartLocation = m_projectile->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_projectile->GetActorLocation();
    EndLocation.Z += m_connectingRange;
 
    /*Search for static objects only*/
    ECollisionChannel ECC = ECollisionChannel::ECC_WorldStatic;
 
    /*Declare the Collision Shape, assign a Sphere shape and set it's radius*/
    FCollisionShape CollisionShape;
    CollisionShape.ShapeType = ECollisionShape::Sphere;
    CollisionShape.SetSphere(m_connectingRange);
	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(m_projectile);

	FCollisionResponseParams test;
	test = ECollisionResponse::ECR_Block;

    /*Perform the actual raycast. This method returns true if there is at least 1 hit.*/
    bool bHitSomething = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::FQuat(), ECC, CollisionShape, queryParams,test);
	TSet<AActor*> actorsInRangeSet;
	TArray<AActor*> actorsInRange;
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

        for (auto It = HitResults.CreateIterator(); It; It++)
        {
			AActor* hitActor = (*It).Actor.Get();
			
			int tagIndex = 0;
			for (auto actorTag : hitActor->Tags)
			{
				if (actorTag == m_connectingTags[tagIndex])
				{
					tagIndex = tagIndex + 1 % m_connectingTags.Num();
					ignoredActors.Add(hitActor);
					if (!CheckTargetIsBlocked(hitActor, ignoredActors))
					{
						GLog->Log(hitActor->GetName());
						actorsInRangeSet.Add(hitActor);
					}
				}
			}
        }
    }

	// Sorts set so nearest actor is on index 0
	actorsInRangeSet.Sort([this](AActor& A, AActor& B) 
	{
		float absDistanceA = FMath::Abs(FVector::Dist(A.GetActorLocation(), m_projectile->GetActorLocation()));
		float absDistanceB = FMath::Abs(FVector::Dist(B.GetActorLocation(), m_projectile->GetActorLocation()));

		return absDistanceA < absDistanceB; // sort by reverse-alphabetical order
	});
	// converts set to array
	actorsInRange = actorsInRangeSet.Array();


    /*In order to draw the sphere of the first image, I will use the DrawDebugSphere function which resides in the DrawDebugHelpers.h
    This function needs the center of the sphere which in this case is provided by the following equation*/
    FVector CenterOfSphere = ((EndLocation - StartLocation) / 2) + StartLocation;
 
    /*Draw the sphere in the viewport*/
    // UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CenterOfSphere, CollisionShape.GetSphereRadius(), 100, FColor::Green, true);

	return actorsInRange;
}

void UConnectingProjectileComponent::OnConnectRequest(AActor* sender, AActor* receiver, float distance)
{
	if (receiver == GetOwner() && sender != nullptr)
	{
		if (distance >= m_minConnectionDistance && m_connectionLeft > 0)
		{
			// ---------- If condition is true then tell requester that he can make a connection to this actor and tell him how many connections are left and the lifeTime
			m_connectionLeft--;
			m_eventSystem->OnProjectileConnectionReply.Broadcast(sender, true, m_connectedTime, m_connectionLeft);
		}
		else
		{
			// ---------- If not then tell actor that he cannot connect
			m_eventSystem->OnProjectileConnectionReply.Broadcast(sender, false, 0, -1);
		}
	}
}

void UConnectingProjectileComponent::OnConnectReply(AActor* receiver, bool canConnect, float connectDuration, int connectionsLeft)
{
	if (receiver != GetOwner())
		return;
	if (canConnect)
	{
		FVector spawnLoc = m_projectile->GetActorLocation();
		FVector dist = m_connectedActor->GetActorLocation() - m_projectile->GetActorLocation();

		// Set connections left to existing projectile connection construct
		m_connectionLeft = connectionsLeft;

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(m_projectile->GetActorLocation(), m_connectedActor->GetActorLocation());
		FRotator spawnRot = lookAtRot;

		m_beam = GetWorld()->SpawnActor<AShootergameBeam>(m_connectingBeamClass, spawnLoc, spawnRot, spawnParams);
		m_beam->InitBeam(m_damage, m_shooterType, m_moduleType, m_damageCauser, m_damageTypeClass);
		/*m_beam->SetLifeSpan(m_connectedTime);

		m_projectile->SetLifeSpan(m_connectedTime);
		m_connectedActor->SetLifeSpan(m_connectedTime);*/

		float distSize = dist.Size();

		m_beam->SetBeamEndPos(FVector(distSize, 0, 0));
		m_beam->SetBeamBoxLength(distSize);

		m_eventSystem->OnNotifyProjectileConnecter.Broadcast(m_connectedActor, connectDuration, m_connectionLeft);
		m_eventSystem->OnNotifyConnectedProjectile.Broadcast(m_connectedActor, connectDuration, m_connectionLeft);
	}
	else
		m_connectedActor = nullptr;
	
}

void UConnectingProjectileComponent::OnNotifyProjectileConnecter(AActor* projectileConnecter, float lifeTime, int connectionsLeft)
{
	if (projectileConnecter == GetOwner())
	{
		UpdateProjectileBeamParams(lifeTime, connectionsLeft);
		if (!m_connectedActor)
			return;
		m_eventSystem->OnNotifyProjectileConnecter.Broadcast(m_connectedActor, lifeTime, m_connectionLeft);
		m_eventSystem->OnNotifyConnectedProjectile.Broadcast(m_connectedActor, lifeTime, m_connectionLeft);
	}
}

void UConnectingProjectileComponent::OnNotifyConnectedProjectile(AActor* connectedProjectile, float lifeTime, int connectionsLeft)
{
	if (!m_connectedActor)
		return;
	if (connectedProjectile == m_connectedActor)
	{
		UpdateProjectileBeamParams(lifeTime, connectionsLeft);
		m_eventSystem->OnNotifyConnectedProjectile.Broadcast(GetOwner(), lifeTime, m_connectionLeft);
	}
}

void UConnectingProjectileComponent::UpdateProjectileBeamParams(float lifeTime, int connectionsleft)
{
	if(m_projectile)
			m_projectile->SetLifeSpan(lifeTime);
		/*if(m_beam)
			m_beam->SetLifeSpan(lifeTime);*/
		m_connectionLeft = connectionsleft;
}