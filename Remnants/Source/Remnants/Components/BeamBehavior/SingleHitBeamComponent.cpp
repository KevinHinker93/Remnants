// Fill out your copyright notice in the Description page of Project Settings.

#include "SingleHitBeamComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "../../Core/Items/ShootergameBeam.h"
#include "Engine/World.h"
#include "EventSystemHandlerSingleton.h"

void USingleHitBeamComponent::InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType)
{
	Super::InitBehaviour(damage, damageCauser, damageTypeClass, moduleType);

	checkf(m_energyLossAmount > 0.0f, TEXT("Beam energy loss amount cannot be  <= 0, have you forgot to set it properly in editor"));
	float loseCount = m_currentBeamSize.GetMax() / m_energyLossAmount;
	checkf(loseCount > 0.0f, TEXT("Beam lose count <= 0"));
	m_loseEnergyInterval = m_loseEnergyDuration / loseCount;
	GetWorld()->GetTimerManager().SetTimer(m_beamLoseEnergyTimerHandle, this, &USingleHitBeamComponent::OnLoseEnergy, m_loseEnergyTime, false);
}

void USingleHitBeamComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass)
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(m_damageIntervalHandle))
		m_timerDelegate.BindUFunction(this, FName("OnDamageEnemy"), DamagedActor, HitFromDirection, HitInfo, NULL);
	else
		return;

	OnDamageEnemy(DamagedActor, HitFromDirection, HitInfo, EventInstigator);
	GetWorld()->GetTimerManager().SetTimer(m_damageIntervalHandle, m_timerDelegate, m_damageInterval, true);
}
void USingleHitBeamComponent::OnDeath()
{
	Super::OnDeath();

	GetWorld()->GetTimerManager().ClearTimer(m_damageIntervalHandle);
}

void USingleHitBeamComponent::OnOverlapEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(m_damageIntervalHandle);
}

void USingleHitBeamComponent::OnApplyDamage(AActor* damagedActor, float baseDamage, FVector const& hitFromDirection, FHitResult const& hitInfo,
	AController* eventInstigator, AActor* damageCauser, TSubclassOf<UDamageType> damageTypeClass)
{
	UGameplayStatics::ApplyPointDamage(damagedActor, baseDamage, hitFromDirection, hitInfo, NULL, damageCauser, damageTypeClass);
}

void USingleHitBeamComponent::OnDamageEnemy(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator)
{
	if(damagedActor)
		if (damagedActor->IsPendingKill())
			return;

	TArray<AActor*> ignoredActors;
	ignoredActors.Add(m_damageCauser);
	ignoredActors.Add(Cast<AActor>(m_beam));
	ignoredActors.Add(damagedActor);

	if (!CheckTargetIsBlocked(damagedActor, ignoredActors))
	{
		OnApplyDamage(damagedActor, m_beamDamage * m_damageMult, HitFromDirection, HitInfo, NULL, m_damageCauser, m_damageTypeClass);
		
		if (!damagedActor->bCanBeDamaged)
			return;
		if(m_showBeamExplosion)
			m_beam->OnHitExplode(damagedActor->GetActorLocation());
	}
}

void USingleHitBeamComponent::OnLoseEnergy()
{
	UE_LOG(LogTemp, Warning, TEXT("On lose energy: %f"), m_energyLossAmount);
	FVector energyLossVec = FVector(m_energyLossAmount);
	m_currentBeamSize -= energyLossVec;
	/*FVector changedSize = FVector(
			FMath::Max(m_currentBeamSize.X - m_energyLossAmount, 0.0f), FMath::Max(m_currentBeamSize.Y - m_energyLossAmount, 0.0f), FMath::Max(m_currentBeamSize.Z - m_energyLossAmount, 0.0f));*/
	m_currentBeamSize = FVector(FMath::Max(m_currentBeamSize.X, 0.0f), FMath::Max(m_currentBeamSize.Y, 0.0f), FMath::Max(m_currentBeamSize.Z, 0.0f));
	m_beam->ChangeParticleSystemSize(m_currentBeamSize);
	GetWorld()->GetTimerManager().ClearTimer(m_beamLoseEnergyTimerHandle);
	if(m_currentBeamSize.Size() > 0.0f)
		GetWorld()->GetTimerManager().SetTimer(m_beamLoseEnergyTimerHandle, this, &USingleHitBeamComponent::OnLoseEnergy, m_loseEnergyInterval, false);
	else
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnExternalEndFiring.Broadcast(m_damageCauser, m_firedModuleType);
	}
}