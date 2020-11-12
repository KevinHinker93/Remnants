// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../ElementDamageType.h"
#include "ProjectileBehaviourComponentBase.generated.h"

class AController;
class AShootergameProjectile;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), ABSTRACT)
class REMNANTS_API UProjectileBehaviourComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileBehaviourComponentBase();

	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
			TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range);
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator);
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator) { check(0 && "You must override this"); }
	virtual void EndCollision() { check(0 && "You must override this"); }
	virtual void OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult,
			int elementLvL, float explosionRadiusMult);
	void SetDamage(float newDamage) { m_damage = newDamage; }
protected:
	UPROPERTY(BlueprintReadOnly)
		AShootergameProjectile* m_projectile;
	UPROPERTY()
		UProjectileMovementComponent* m_projectileMovement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_particleExplosionSizeMult;

	float m_projectileRange;
	
	float m_damage;
	FName m_shooterType;
	int m_moduleType;
	UPROPERTY()
		AActor* m_damageCauser;
	UPROPERTY()
		TSubclassOf<UElementDamageType> m_damageTypeClass;

	float CalcLifeTimeFromProjectileRange(float range);
	float CalcTotalDamage(float initialDamage);
	bool CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors);
	
};
