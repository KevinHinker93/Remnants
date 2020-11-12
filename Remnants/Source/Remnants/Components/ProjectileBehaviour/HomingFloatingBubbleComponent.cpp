// Fill out your copyright notice in the Description page of Project Settings.

#include "HomingFloatingBubbleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../../Core/Items/ShootergameProjectile.h"

void UHomingFloatingBubbleComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator)
{
	Super::DamageEnemy(DamagedActor, HitFromDirection, HitInfo,	EventInstigator);
}

void UHomingFloatingBubbleComponent::EndCollision()
{

}

void UHomingFloatingBubbleComponent::OnStartFloating()
{
	UFloatingBubbleComponent::OnStartFloating();
	m_homingCheckColl = NewObject<USphereComponent>(m_projectile, "HomingCheckRadius");
	if (!m_homingCheckColl)
		return;

	FTransform transform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
	m_homingCheckColl->SetWorldTransform(transform);
	m_homingCheckColl->InitSphereRadius(m_homingCheckRadius);

	FName projectileCollProfileName = m_projectile->GetCollisionComp()->GetCollisionProfileName();
	m_homingCheckColl->SetCollisionProfileName(projectileCollProfileName);

	m_homingCheckColl->AttachToComponent(m_projectile->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	m_homingCheckColl->RegisterComponent();
	m_homingCheckColl->OnComponentBeginOverlap.AddDynamic(this, &UHomingFloatingBubbleComponent::OnHomingOverlapBegin);
}

void UHomingFloatingBubbleComponent::OnHomingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (m_projectileMovement->bIsHomingProjectile == false)
		OnStartHoming(OtherActor);
}

void UHomingFloatingBubbleComponent::OnStartHoming(AActor* actorToHomeTo)
{
	if (!m_projectileMovement || !actorToHomeTo)
		return;
	m_projectileMovement->InitialSpeed /= m_homingSpeedDiv;
	m_projectileMovement->MaxSpeed /= m_homingSpeedDiv;
	m_projectileMovement->HomingTargetComponent = actorToHomeTo->GetRootComponent();
	m_projectileMovement->HomingAccelerationMagnitude = m_homingMagnitude;
	m_projectileMovement->bIsHomingProjectile = true;
}



