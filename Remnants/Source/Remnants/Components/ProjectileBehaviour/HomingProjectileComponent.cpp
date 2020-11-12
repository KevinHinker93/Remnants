// Fill out your copyright notice in the Description page of Project Settings.

#include "HomingProjectileComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "../../Core/Characters/ShootergameCharacter.h"
#include "../../Core/Characters/EmptyActor.h"
#include "../../AbleToShoot.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"

void UHomingProjectileComponent::InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range)
{
	Super::InitBehaviour(damage, shooterType, moduleType, projectileSpawner, damageTypeClass, projectileMovement, range);

	m_homingCheckColl = NewObject<USphereComponent>(m_projectile, "HomingCheckRadius");
	if (!m_homingCheckColl)
		return;

	FTransform transform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
	m_homingCheckColl->SetWorldTransform(transform);
	m_homingCheckColl->InitSphereRadius(m_homingCheckRadius);
	/*m_homingCheckColl->SetVisibility(true);
	m_homingCheckColl->SetHiddenInGame(false);*/
	FName projectileCollProfileName = m_projectile->GetCollisionComp()->GetCollisionProfileName();
	m_homingCheckColl->SetCollisionProfileName(projectileCollProfileName);
	

	m_homingCheckColl->AttachToComponent(m_projectile->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	m_homingCheckColl->RegisterComponent();
	m_homingCheckColl->OnComponentBeginOverlap.AddDynamic(this, &UHomingProjectileComponent::OnHomingOverlapBegin);
}

void UHomingProjectileComponent::OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator)
{
	Super::OnHit(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
}

void UHomingProjectileComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	m_projectile->OnExplode(HitInfo.ImpactPoint, true, m_particleExplosionSizeMult);

	UGameplayStatics::ApplyPointDamage(DamagedActor, CalcTotalDamage(m_damage), HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
	
}

void UHomingProjectileComponent::EndCollision()
{

}

void UHomingProjectileComponent::OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult)
{
	Super::OnRangeEnd(shooter, moduleCriticalChance, moduleCriticalDamageMult, elementLvL, explosionRadiusMult);
}

void UHomingProjectileComponent::OnHomingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if(m_projectileMovement->bIsHomingProjectile == false)
		OnStartHoming(OtherActor);
}

void UHomingProjectileComponent::OnStartHoming(AActor* actorToHomeTo)
{
	if (!m_projectileMovement || !actorToHomeTo)
		return;
	m_projectileMovement->InitialSpeed /= m_homingSpeedDiv;
	m_projectileMovement->MaxSpeed /= m_homingSpeedDiv;
	m_projectileMovement->HomingTargetComponent = actorToHomeTo->GetRootComponent();
	m_projectileMovement->HomingAccelerationMagnitude = m_homingMagnitude;
	m_projectileMovement->bIsHomingProjectile = true;
}


