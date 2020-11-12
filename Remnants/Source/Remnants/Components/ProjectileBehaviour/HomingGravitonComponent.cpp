// Fill out your copyright notice in the Description page of Project Settings.

#include "HomingGravitonComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void UHomingGravitonComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	Super::DamageEnemy(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UHomingGravitonComponent::EndCollision()
{
	Super::EndCollision();
}

void UHomingGravitonComponent::OnStartFloating()
{
	UFloatingBubbleComponent::OnStartFloating();

	GetWorld()->GetTimerManager().SetTimer(m_gravitonIntervalHandle, this, &UHomingGravitonComponent::OnGraviton, m_gravitonInterval, true);
}

void UHomingGravitonComponent::OnGraviton()
{
	TArray<AActor*> actorsToDamage = GetActorsInRange();
	for (int i = 0; i < actorsToDamage.Num(); i++)
	{
		FTimerHandle timerHandle;
		FTimerDelegate timerDelegate;
		AActor* nextActorToShotAt = actorsToDamage[i];
		if (!nextActorToShotAt)
			continue;
		FGravitonTimerData gravitonData;
		gravitonData.m_handle = timerHandle;
		gravitonData.m_timerDelegate = timerDelegate;
		int insertedIdx = m_timerData.Add(gravitonData);

		int randSeed = FMath::Rand();
		FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
		float gravitonTimer = FMath::RandRange(m_minRandomShotTime, m_maxRandomShotTime);

		timerDelegate.BindUFunction(this, FName("OnShootGravitonOnEnemy"), nextActorToShotAt, insertedIdx);
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, gravitonTimer, false);
	}
}

void UHomingGravitonComponent::OnShootGravitonOnEnemy(AActor* actorToShootTo, int insertedIdx)
{
	if (!actorToShootTo || actorToShootTo->IsPendingKill())
		return;

	FVector spawnLoc = GetOwner()->GetActorLocation();
	FRotator spawnRot = FRotator::ZeroRotator;
	spawnRot = UKismetMathLibrary::FindLookAtRotation(spawnLoc, actorToShootTo->GetActorLocation());

	UClass* projClass = nullptr;
	if (m_gravitonAlternativeProjectile)
		projClass = m_gravitonAlternativeProjectile;
	else
		projClass = m_projectile->GetClass();

	if (!projClass)
		return;

	if (!m_damageCauser)
		return;
	if (m_damageCauser->Tags.Num() <= 0)
		return;

	UHomingShotgunStrategy* shotgunStrat = m_gravitonShotFiringStrat.GetDefaultObject();
	if (shotgunStrat)
		shotgunStrat = DuplicateObject(shotgunStrat, shotgunStrat->GetOuter());
	else
		return;

	int randSeed = FMath::Rand();
	FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
	//float coneOffset = FMath::RandRange(0.05f, m_gravitonRotOffset);

	shotgunStrat->SetRotOffset(m_gravitonRotOffset);
	shotgunStrat->SetNumProjectiles(m_gravitonNumProjectiles);
	shotgunStrat->SetHomingTargetActor(actorToShootTo, m_homingMagnitude);

	/*FRotator rot = spawnRot;
	rot.Yaw += m_gravitonRotOffset;
	rot.Pitch += m_gravitonRotOffset;*/

	shotgunStrat->FiringLogic(m_damageCauser, spawnLoc, spawnRot, GetOwner()->GetWorld(), m_damage * m_gravitonShotDamageMult, 0.0f, 1.0f, 1,
		0.0f, 1.0f, m_gravitonShotSizeMult,
		m_homingSpeedDiv, projClass, NULL, m_damageTypeClass, m_damageCauser->Tags[0], -1, m_gravitonProjSound);

	//m_timerData.RemoveAt(insertedIdx);
	//FTransform spawnTransform(spawnRot.Quaternion(), spawnLoc, FVector(1, 1, 1));

	//AShootergameProjectile* proj = GetWorld()->SpawnActorDeferred<AShootergameProjectile>(projClass, spawnTransform,
	//		nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	//if (proj)
	//{
	//	proj->InitProjectile(m_damage * m_gravitonShotDamageMult, 0.0f, 0.0f, m_homingSpeedDiv, m_gravitonShotSizeMult,
	//			1.0f, 0.0f, "Player", -1, m_damageCauser, m_damageTypeClass);
	//	

	//	UGameplayStatics::FinishSpawningActor(proj, spawnTransform);
	//}
}

TArray<AActor*> UHomingGravitonComponent::GetActorsInRange()
{
	/*TArray is the collection that contains all the HitResults*/
	TArray<FHitResult> HitResults;

	/*The Start location of the sphere*/
	FVector StartLocation = m_projectile->GetActorLocation();

	/*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
	To extend this functionality, you can replace the height variable with a FVector*/
	FVector EndLocation = m_projectile->GetActorLocation();
	EndLocation.Z += m_gravitonRange;

	ECollisionChannel ECC = ECollisionChannel::ECC_MAX;

	/*Declare the Collision Shape, assign a Sphere shape and set it's radius*/
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(m_gravitonRange);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(m_projectile);

	FCollisionResponseParams test;
	test = ECollisionResponse::ECR_Block;

	/*Perform the actual raycast. This method returns true if there is at least 1 hit.*/
	bool bHitSomething = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::FQuat(), ECC, CollisionShape, queryParams, test);
	TArray<AActor*> actorsInRange;
	/*If the raycast hit a number of objects, iterate through them and print their name in the console*/
	if (bHitSomething)
	{
		for (auto It = HitResults.CreateIterator(); It; It++)
		{
			if (!(*It).Actor.IsValid())
				continue;
			AActor* hitActor = (*It).Actor.Get();
			if (!hitActor || hitActor->IsPendingKill())
				continue;
			if ((hitActor->ActorHasTag("Enemy") || hitActor->ActorHasTag("Destructible") || hitActor->ActorHasTag("Loot")) && !CheckTargetIsBlocked(hitActor, hitActor))
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

bool UHomingGravitonComponent::CheckTargetIsBlocked(AActor* target, AActor* lastHitActor)
{
	FHitResult HitResult;

	/*The Start location of the sphere*/
	FVector StartLocation = target->GetActorLocation();

	/*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
	To extend this functionality, you can replace the height variable with a FVector*/
	FVector EndLocation = m_projectile->GetActorLocation();

	TArray<AActor*> ignoredActors;
	ignoredActors.Add(target);
	ignoredActors.Add(m_projectile);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActors(ignoredActors);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, queryParams);
	if (bHitSomething)
		GLog->Log(HitResult.GetActor()->GetName());
	return bHitSomething;
}