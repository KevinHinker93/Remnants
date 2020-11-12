// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "PiercingShotComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UPiercingShotComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
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
		int m_piercingCount; // Number of hits which dont destroy projectile
	// How often projectile can hurt entity when inside collider multiple times
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		int m_multipleHits; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_multipleHitInterval;

private:
	UPROPERTY()
		TArray<AActor*> m_hitActors;

	FTimerDelegate m_timerDelegate;
	FTimerHandle m_multipleHitIntervalTimerHandle;

	UFUNCTION()
		void OnMultipleHit(FVector const& HitFromDirection, FHitResult const& HitInfo,
				AController* EventInstigator);	
};
