// Fill out your copyright notice in the Description page of Project Settings.

#include "PullingBeamComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "EventSystemHandlerSingleton.h"
#include "../../Core/Characters/ShootergameCharacter.h"
#include "../WeaponComponent.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "AbleToShoot.h"
#include "Camera/CameraComponent.h"

void UPullingBeamComponent::UpdatePullDirection()
{
	if (m_actorToPull)
	{
		if (m_actorToPull->IsPendingKill())
			return;
	}
	else if (m_enemyToPull)
		if (m_enemyToPull->IsPendingKill())
			return;

	FVector direction = m_playerCamera->GetForwardVector();
	direction.Normalize();

	float desiredPullStrenght = CalcDesiredPullStrength();

	if (desiredPullStrenght < m_pullThreshold)
		return;

	if (m_enemyToPull)
	{
		FVector pullEndPoint = (-direction * desiredPullStrenght) + m_enemyToPull->GetActorLocation();
		PullEnemy.Broadcast(m_enemyToPull->GetActorLocation(), pullEndPoint, m_enemyToPull);
	}
	else if(m_actorToPull)
	{
		FVector pullEndPoint = (-direction * desiredPullStrenght) + m_actorToPull->GetActorLocation();
		PullEnemy.Broadcast(m_actorToPull->GetActorLocation(), pullEndPoint, m_actorToPull);
	}

}

void UPullingBeamComponent::AbsorbProjectile(AShootergameProjectile* projectile)
{
	m_absorbedProjectileCount += 1;
	m_damageMult = m_initialDamageMult * float(m_absorbedProjectileDamageMult * m_absorbedProjectileCount);
	if (!projectile)
		return;
	accumulatedAbsorbedDamageFromProjectiles += projectile->GetProjectileBaseDamage();
}

void UPullingBeamComponent::InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType)
{
	Super::InitBehaviour(damage, damageCauser, damageTypeClass, moduleType);

	m_initialDamageMult = m_damageMult;
	accumulatedAbsorbedDamageFromProjectiles = 0.0f;
	m_died = false;
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Player", actors);

	AShootergameCharacter* player = Cast<AShootergameCharacter>(actors[0]);
	m_playerCamera = player->GetFirstPersonCameraComponent();
	ResetPulledObjects();
}

void UPullingBeamComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass)
{
	Super::DamageEnemy(DamagedActor, HitFromDirection, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);

	if (!DamagedActor || DamagedActor->IsPendingKill())
		return;

	FVector direction = m_playerCamera->GetForwardVector();
	direction.Normalize();

	AAEnemyDrone* enemy = Cast<AAEnemyDrone>(DamagedActor);
	if (enemy)
	{
		m_enemyToPull = enemy;

		float desiredPullStrenght = CalcDesiredPullStrength();

		if (desiredPullStrenght < m_pullThreshold)
			return;

		FVector pullEndPoint = (-direction * desiredPullStrenght) + DamagedActor->GetActorLocation();
		PullEnemy.Broadcast(DamagedActor->GetActorLocation(), pullEndPoint, DamagedActor);
	}
	else if (DamagedActor->ActorHasTag("PullAble"))
	{
		m_actorToPull = DamagedActor;
		float desiredPullStrenght = CalcDesiredPullStrength();

		if (desiredPullStrenght < m_pullThreshold)
			return;

		FVector pullEndPoint = (-direction * desiredPullStrenght) + DamagedActor->GetActorLocation();
		PullEnemy.Broadcast(DamagedActor->GetActorLocation(), pullEndPoint, DamagedActor);

	}
}
void UPullingBeamComponent::OnDeath()
{
	Super::OnDeath();
	if (m_enemyToPull == nullptr && !m_died && m_absorbedProjectileCount >= 1)
	{
		ResetPulledObjects();
		m_died = true;
		UE_LOG(LogTemp, Log, TEXT("Try to shoot back %d projectiles"), m_absorbedProjectileCount);

		IAbleToShoot* shooter = Cast<IAbleToShoot>(m_damageCauser);
		if (shooter)
		{
			UWeaponComponent* weaponComp = Cast<UWeaponComponent>(m_damageCauser->GetComponentByClass(UWeaponComponent::StaticClass()));
			FRotator spawnRot = weaponComp->GetComponentRotation();
			FVector spawnLoc = weaponComp->GetComponentLocation();

			UShotgunStrategy* shotgunStrat = m_counterShotgunFiringStrat.GetDefaultObject();
			if (shotgunStrat)
				shotgunStrat = DuplicateObject(shotgunStrat, shotgunStrat->GetOuter());
			else
				return;

			if (!m_damageCauser)
				return;
			if (m_damageCauser->Tags.Num() <= 0)
				return;

			float baseDamage = m_beamDamage * m_damageMult * m_absorbedProjectileCount * m_counterProjectileDamageMult;
			baseDamage += accumulatedAbsorbedDamageFromProjectiles * m_absorbedProjectileDamageMult;

			shotgunStrat->SetRotOffset(m_firedConeOffset);
			int numProjectiles = FMath::Min(m_maxProjectiles, m_absorbedProjectileCount);
			shotgunStrat->SetNumProjectiles(numProjectiles);
			float damagePerProjectile = baseDamage / numProjectiles;

			shotgunStrat->FiringLogic(m_damageCauser, spawnLoc, spawnRot, GetOwner()->GetWorld(), damagePerProjectile, 0.0f, 1.0f, 1,
					m_counterRange, 1.0f, m_counterProjectileSizeMult,
					1.0f, m_projectile, NULL, m_damageTypeClass, m_damageCauser->Tags[0], 0, m_counterProjSound);

			
			shooter->WeaponFireAnimation();
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Actor is not able to shoot"));
	}
	else
	{
		ResetPulledObjects();
		m_died = true;
	}
}

void UPullingBeamComponent::OnOverlapEnd()
{
	ResetPulledObjects();
}

float UPullingBeamComponent::CalcDesiredPullStrength()
{
	if (m_enemyToPull)
	{
		float enemyMass = m_enemyToPull->PushPullResistance();
		UE_LOG(LogTemp, Verbose, TEXT("Actor mass: %f"), enemyMass);
		checkf(enemyMass > 0, TEXT("Zero mass / pushPullResistance not allowed, have you forgot to set it?"));
		return m_pullStrength / enemyMass;
	}
	else
		return m_pullStrength / m_defaultPullMass;

}

void UPullingBeamComponent::OnPullEnemy(AActor* actorToPull)
{

}

void UPullingBeamComponent::ResetPulledObjects()
{
	m_enemyToPull = nullptr;
	m_actorToPull = nullptr;
}