// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "KnockbackProjectileComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKnockBackEnemy, FVector, start, FVector, end, AActor*, actorToKnockback);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UKnockbackProjectileComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
	// Knockbacks enemy from a given start point to an end point
	UPROPERTY(BlueprintAssignable, Category= "KnockbackEnemy")
		FOnKnockBackEnemy KnockbackEnemy;

	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
			TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator) override;
	virtual void EndCollision() override;
	virtual void OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_knockbackStrenght;
	// Minimum pushing value to push enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_pushThreshold;
};
