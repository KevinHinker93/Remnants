// Fill out your copyright notice in the Description page of Project Settings.

#include "SpreadingExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../EventSystemHandlerSingleton.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Engine/World.h"

void USpreadingExplosionComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner,
	TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	bool isValid = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(isValid);
	if (isValid)
		eventHandler->OnSpreadExplosion.AddDynamic(this, &USpreadingExplosionComponent::OnSpreadExplosion);
}

void USpreadingExplosionComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	Super::DamageEnemy(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void USpreadingExplosionComponent::EndCollision()
{
	Super::EndCollision();
}

void USpreadingExplosionComponent::ApplyRadialDamage(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	// Logic here
	m_projectile->OnExplode(m_projectile->GetActorLocation(), true, m_particleExplosionSizeMult * m_explosionRadius);
	m_firstHitActor = damagedActor;
	
	TArray<AActor*> actorsToDamage = GetActorsInRange();
	// Store already damaged enemies to avoid multiple damage
	TSet<AActor*> damagedActors;
	damagedActors.Append(m_ignoringActors);
	for (auto actor : actorsToDamage)
	{
		// The actor was already damaged ignore it
		if (damagedActors.Contains(actor))
			continue;
		UGameplayStatics::ApplyPointDamage(actor, CalcTotalDamage(m_damage * m_damageMult), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
		UGameplayStatics::ApplyPointDamage(actor, CalcTotalDamage(m_damage * m_damageMult), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
		damagedActors.Add(actor);
		if(actor->Tags.Num() > 0 && actor->ActorHasTag("enemy") && actor != m_firstHitActor)
			SpreadExplosionTo(actor);
	}

	bool isValid = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(isValid);
	if (isValid)
		eventHandler->OnSpreadExplosion.Broadcast(damagedActors);
}

void USpreadingExplosionComponent::SpreadExplosionTo(AActor* actorToSpreadTo)
{
	if (!actorToSpreadTo)
		return;
	FVector spawnLoc = actorToSpreadTo->GetActorLocation();
	FRotator spawnRot = actorToSpreadTo->GetActorRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* owner = GetOwner();
	UWorld* world = owner->GetWorld();
	if (!world || !owner)
		return;
	// spawn the projectile at the muzzle
	AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(owner->GetClass(), spawnLoc, spawnRot, spawnParameters);
	proj->InitProjectile(m_damage, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, "Player", 1, m_damageCauser, m_damageTypeClass);
}

void USpreadingExplosionComponent::OnSpreadExplosion(TSet<AActor*> actorsReceivedExplosion)
{
	m_ignoringActors.Append(actorsReceivedExplosion);
	/*for (int i = 0; i < ignoredActors.Num(); i++)
	{
		if (!m_ignoringActors.Contains(ignoredActors[i]))
			m_ignoringActors.Add(ignoredActors[i]);
	}*/
}