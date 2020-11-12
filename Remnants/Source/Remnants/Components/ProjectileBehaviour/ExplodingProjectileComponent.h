// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "TimerManager.h"
#include "ExplodingProjectileComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplode);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UExplodingProjectileComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category= "OnExplode")
		FOnExplode OnExplode;

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
	// Damage radius explosion, defines base particle explosion radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_explosionRadius;
	// Set to zero for no influence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_gravityInfluence; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_explodingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_projectileSpeedMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_sizeMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageMult;

	UPROPERTY()
		AActor* m_firstHitActor;

	virtual void ApplyRadialDamage(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator);

	TArray<AActor*> GetActorsInRange();
	bool CheckTargetIsBlocked(AActor* target, AActor* lastHitActor);

private:
	FTimerHandle m_explosionTimerHandle;
	FTimerDelegate m_timerDelegate;

	UFUNCTION()
		void Explosion(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
				AController* EventInstigator);

	UFUNCTION()
		void OnAttachedEnemyDestroyed(AActor* destroyedActor);

	void PrepareExplosion(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator);
	
};
