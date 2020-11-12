// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserbeamComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "../../Core/Items/ShootergameBeam.h"
#include "EventSystemHandlerSingleton.h"
#include "Engine/World.h"

void ULaserbeamComponent::InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType)
{
	Super::InitBehaviour(damage, damageCauser, damageTypeClass, moduleType);

	m_beam->SetLifeSpan(m_beamLifeTime);
}

void ULaserbeamComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(m_timeToDamageHandle))
		m_timerDelegate.BindUFunction(this, FName("OnDamageEnemy"), DamagedActor, HitFromDirection, HitInfo, NULL);
	else
		return;

	m_beam->SetLifeSpan(0.0f);
	GetWorld()->GetTimerManager().SetTimer(m_timeToDamageHandle, m_timerDelegate, m_timeToDamage, false);
}
void ULaserbeamComponent::OnDeath()
{
	Super::OnDeath();
}

void ULaserbeamComponent::OnOverlapEnd()
{

}

void ULaserbeamComponent::OnDamageEnemy(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	if (!damagedActor->IsPendingKill())
	{

		TArray<AActor*> ignoredActors;
		ignoredActors.Add(m_damageCauser);
		ignoredActors.Add(m_beam);
		ignoredActors.Add(damagedActor);

		if (!CheckTargetIsBlocked(damagedActor, ignoredActors))
		{
			UGameplayStatics::ApplyPointDamage(damagedActor, m_beamDamage * m_damageMult, HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
			if (damagedActor->bCanBeDamaged)
				m_beam->OnHitExplode(damagedActor->GetActorLocation());
		}
	}

	m_beam->Destroy();
}
