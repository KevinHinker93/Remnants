// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "FloatingBubbleComponent.generated.h"

class UProjectileMovementComponent;
class AController;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UFloatingBubbleComponent : public UProjectileBehaviourComponentBase
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
	// Distance the projectile begins to float
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings", meta = (UIMin = "1.0", UIMax = "4.0"))
		float m_startToFloatDist; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_floatingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_sizeMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_interpolationFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_floatingGravityScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageMult;
	// if bubbles can explode randomly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_randomLifetimeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_randomLifetimeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extra Behaviour Settings")
		bool m_isMultihit = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extra Behaviour Settings")
		float m_hitInterval = 0.2f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnStartFloating();

private:
	FTimerHandle m_speedTimerHandle;
	float m_currentBubbleSize;
	float m_desiredBubbleSize;
	bool m_interpolate;
	float m_absSizeDiff;
	float m_sizeDiff;

	void InterpolateBubbleSize();

	FTimerDelegate m_timerDelegate;
	FTimerHandle m_multipleHitIntervalTimerHandle;

	UFUNCTION()
		void OnMultipleHit(FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator);
};
