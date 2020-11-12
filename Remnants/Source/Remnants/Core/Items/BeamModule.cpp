// Fill out your copyright notice in the Description page of Project Settings.

#include "BeamModule.h"
#include "ShootergameBeam.h"
#include "AbleToShoot.h"

UBeamModule::UBeamModule()
{
	m_firing = false;
}

void UBeamModule::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType)
{
	//if (world != NULL)
	//{
	//	if (m_firing == false)
	//	{
	//		//Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//		AWeaponElement* moduleElement = m_ElementClassDummy->GetDefaultObject<AWeaponElement>();
	//		TSubclassOf<AShootergameBeam> beamClass = element->GetBeam();

	//		float moduleDamMult = GetDamageMult();
	//		float elementDamMult = element->GetElementalDamageMult();
	//		float totalDamageMult = elementDamMult * moduleDamMult;

	//		int totalDamage = damage * totalDamageMult * damageMult;

	//		if (!beamClass)
	//		{
	//			UE_LOG(LogTemp, Error, TEXT("Beam Module has no beam to fire"));
	//			return;
	//		}

	//		// spawn the projectile at the muzzle
	//		m_beam = world->SpawnActor<AShootergameBeam>(beamClass, spawnLoc, spawnRot, ActorSpawnParams);
	//		m_beam->InitBeam(totalDamage, shooter->GetClass());
	//		//m_beam->AttachRootComponentToActor(this);
	//		auto shootingActor = Cast<IAbleToShoot>(shooter);
	//		if (shootingActor)
	//		{
	//			m_beam->AttachToComponent(shootingActor->GetMuzzelLoc(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	//		}
	//		m_firing = true;
	//	}
	//	m_beam->SetCanDamage(true);
	//}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("Beam Module World is null"));
}

void UBeamModule::StopFiring(UWorld* world)
{
	m_firing = false;
	m_beam->Destroy();
}

