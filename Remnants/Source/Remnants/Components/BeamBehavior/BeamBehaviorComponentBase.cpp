// Fill out your copyright notice in the Description page of Project Settings.

#include "BeamBehaviorComponentBase.h"
#include "../../Core/Items/ShootergameBeam.h"
#include "Kismet/GameplayStatics.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "EventSystemHandlerSingleton.h"

// Sets default values for this component's properties
UBeamBehaviorComponentBase::UBeamBehaviorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBeamBehaviorComponentBase::InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType)
{
	AActor* owner = GetOwner();
	AShootergameBeam* beam = Cast<AShootergameBeam>(owner);
	if(beam)
		m_beam = beam;
	m_beamDamage = damage;
	m_damageCauser = damageCauser;
	m_damageTypeClass = damageTypeClass;
	m_firedModuleType = moduleType;
	m_beam->ChangeParticleSystemSize(m_initialBeamSize);
	m_currentBeamSize = m_initialBeamSize;
	GetWorld()->GetTimerManager().SetTimer(m_overheatingIntervalHandle, this, &UBeamBehaviorComponentBase::OnOverheat, m_overheatingInterval, false);
}

void UBeamBehaviorComponentBase::OnDeath()
{
	GetWorld()->GetTimerManager().ClearTimer(m_overheatingIntervalHandle);
	m_beam = nullptr;
}

FVector UBeamBehaviorComponentBase::GetCurrentBeamSize()
{
	return m_currentBeamSize;
}

bool UBeamBehaviorComponentBase::CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors)
{
	FHitResult HitResult;
 
    /*The Start location of the sphere*/
	FVector StartLocation = target->GetActorLocation();
 
    /*The End location of the sphere is equal to the default location of the actor plus the height we will enter from the editor
    To extend this functionality, you can replace the height variable with a FVector*/
    FVector EndLocation = m_beam->GetActorLocation();

	// Get all projectile, since they can prevent overlapping damage
	TArray<AActor*> projectiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShootergameProjectile::StaticClass(), projectiles);

	// Add projectiles to ignore list
	ignoredActors += projectiles;

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActors(ignoredActors);

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, queryParams);
	if(bHitSomething)
		GLog->Log(HitResult.GetActor()->GetName());
	return bHitSomething;
}

void UBeamBehaviorComponentBase::OnOverheat()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnExternalOverheat.Broadcast(m_damageCauser, m_overheatingMult, m_firedModuleType);

	GetWorld()->GetTimerManager().ClearTimer(m_overheatingIntervalHandle);
	GetWorld()->GetTimerManager().SetTimer(m_overheatingIntervalHandle, this, &UBeamBehaviorComponentBase::OnOverheat, m_overheatingInterval, false);
}