// Fill out your copyright notice in the Description page of Project Settings.

#include "WideAreaBeamComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "EventSystemHandlerSingleton.h"

void UWideAreaBeamComponent::InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType)
{
	Super::InitBehaviour(damage, damageCauser, damageTypeClass, moduleType);
	GetWorld()->GetTimerManager().SetTimer(m_damageIntervalHandle, this, &UWideAreaBeamComponent::DamageAllEnemiesInside, m_damageInterval, false);

	if (m_hasImpactOnUserMoveSpeed)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnExternalChangeState.Broadcast(m_damageCauser, "HeavyWeapon");
	}
}

void UWideAreaBeamComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, m_beamDamage * m_damageMult, HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
}
void UWideAreaBeamComponent::OnDeath()
{
	Super::OnDeath();

	if (m_hasImpactOnUserMoveSpeed)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnExternalResetState.Broadcast(m_damageCauser);
	}

	GetWorld()->GetTimerManager().ClearTimer(m_damageIntervalHandle);
}

void UWideAreaBeamComponent::OnOverlapEnd()
{
	
}

void UWideAreaBeamComponent::DamageAllEnemiesInside()
{
	TArray<AActor*> overlappingActors;
	m_beam->GetCollisionComp()->GetOverlappingActors(overlappingActors);
	FVector emptyHitFromDirection = FVector::ZeroVector;
	FHitResult emptyHitResult;

	TArray<AActor*> ignoredActors;
	ignoredActors.Add(m_damageCauser);
	ignoredActors.Add(m_beam);
	ignoredActors += overlappingActors;
	
	for (auto actor : overlappingActors)
	{
		if (!CheckTargetIsBlocked(actor, ignoredActors))
		{
			UGameplayStatics::ApplyPointDamage(actor, m_beamDamage * m_damageMult, emptyHitFromDirection, emptyHitResult, NULL, m_damageCauser, m_damageTypeClass);
			if (!actor->bCanBeDamaged)
				return;
			m_beam->OnHitExplode(actor->GetActorLocation());
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(m_damageIntervalHandle);
	GetWorld()->GetTimerManager().SetTimer(m_damageIntervalHandle, this, &UWideAreaBeamComponent::DamageAllEnemiesInside, m_damageInterval, false);
}
