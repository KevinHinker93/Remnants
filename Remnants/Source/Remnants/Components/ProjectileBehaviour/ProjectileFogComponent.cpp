// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileFogComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "Components/SphereComponent.h"
#include "../../Core/Gameplay/DamageArea.h"
#include "../../EventSystemHandlerSingleton.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UProjectileFogComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);
	m_canSpawnFog = true;
}

void UProjectileFogComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UProjectileFogComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);
	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);

	if(m_canSpawnFog)
		SpawnFog(DamagedActor, EventInstigator, m_damageAreaDuration);

    /*FCollisionShape CollisionShape;
    CollisionShape.ShapeType = ECollisionShape::Sphere;
    CollisionShape.SetSphere(m_damageAreaSize);

    FVector StartLocation = m_projectile->GetActorLocation();
    FVector EndLocation = m_projectile->GetActorLocation();
    EndLocation.Z += m_damageAreaSize;

    FVector CenterOfSphere = ((EndLocation - StartLocation) / 2) + StartLocation;
    UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CenterOfSphere, CollisionShape.GetSphereRadius(), 100, FColor::Green, true);*/
	
}

void UProjectileFogComponent::EndCollision()
{

}

void UProjectileFogComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}

void UProjectileFogComponent::TryAttachToActor(ADamageArea* dmgArea, AActor* actorToAttachTo)
{
	if (actorToAttachTo && m_attachFogToHittedEnemy)
	{
		dmgArea->AttachToComponent(actorToAttachTo->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		dmgArea->OnAreaAttached();
	}
}

void UProjectileFogComponent::SpawnFog(AActor* hittedActor, AController* eventInstigator, float lifeSpan)
{
	if (hittedActor->Tags.Num() <= 0)
		return;

	if (hittedActor->Tags[0] != "Enemy")
		return;

	FTransform spawnTransform(m_projectile->GetActorRotation().Quaternion(), m_projectile->GetActorLocation(), FVector(1, 1, 1));

	ADamageArea* damageArea = GetWorld()->SpawnActorDeferred<ADamageArea>(m_damageArea, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	FName projectileCollisionProfileName = m_projectile->GetCollisionComp()->BodyInstance.GetCollisionProfileName();
	if (damageArea)
	{
		damageArea->SetLifeSpan(lifeSpan);
		damageArea->InitDamageArea(m_damage, m_damageAreaDamagingInterval, eventInstigator, m_damageCauser,
				m_damageTypeClass, projectileCollisionProfileName);
		damageArea->GetRootComponent()->SetWorldScale3D(FVector(m_damageAreaSize, m_damageAreaSize, m_damageAreaSize));

		UGameplayStatics::FinishSpawningActor(damageArea, spawnTransform);

		TryAttachToActor(damageArea, hittedActor);
	}
}

