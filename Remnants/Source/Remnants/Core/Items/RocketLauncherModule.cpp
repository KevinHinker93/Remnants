// Fill out your copyright notice in the Description page of Project Settings.

#include "RocketLauncherModule.h"


URocketLauncherModule::URocketLauncherModule()
{
}

void URocketLauncherModule::FiringLogic(AActor* shooter, const FVector spawnLoc, const FRotator spawnRot, UWorld* const world,
		int damage, float damageMult, float firingSpeed, float firingSpeedMult, UAElement* element, FName shooterType)
{
	//if (world != NULL)
	//{
	//	//Set Spawn Collision Handling Override
	//	FActorSpawnParameters ActorSpawnParams;
	//	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//	auto moduleElement = m_ElementClassDummy->GetDefaultObject<AWeaponElement>();
	//	auto projClass = element->GetProjectile();

	//	float moduleDamMult = GetDamageMult();
	//	float elementDamMult = element->GetElementalDamageMult();
	//	float totalDamageMult = elementDamMult * moduleDamMult;
	//	
	//	int totalDamage = damage * totalDamageMult * damageMult;
	//	float criticalChance = GetCriticalChance();
	//	float criticalDamageMult = GetCriticalMult();

	//	if (!projClass)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("RocketLauncher Module has no proj to fire"));
	//		return;
	//	}
	//	// spawn the projectile at the muzzle
	//	AShootergameProjectile* proj = world->SpawnActor<AShootergameProjectile>(projClass, spawnLoc, spawnRot, ActorSpawnParams);
	//	proj->InitProjectile(totalDamage, criticalChance, criticalDamageMult, element->GetElementalProjectileSpeedMult(), 1,
	//			m_explosionRadiusMult, m_range, shooter->GetClass(), element->GetDamageTypeClass());
	//}
}
void URocketLauncherModule::StopFiring(UWorld* world)
{

}
