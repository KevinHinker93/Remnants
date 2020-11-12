// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ExplodingProjectileComponent.h"
#include "SpreadingExplosionComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API USpreadingExplosionComponent : public UExplodingProjectileComponent
{
	GENERATED_BODY()

public:
	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner,
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator);
	virtual void EndCollision();
	
protected:
	virtual void ApplyRadialDamage(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator);

private:
	TSet<AActor*> m_ignoringActors;

	// Callback for a spreading explosion to update all ignored actors that already received an explosion
	UFUNCTION()
		void OnSpreadExplosion(TSet<AActor*> actorsReceivedExplosion);

	void SpreadExplosionTo(AActor* actorToSpreadTo);
};
