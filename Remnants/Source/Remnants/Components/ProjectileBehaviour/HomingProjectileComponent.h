// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "HomingProjectileComponent.generated.h"

class AEmptyActor;
class USphereComponent;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UHomingProjectileComponent : public UProjectileBehaviourComponentBase
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
		float m_homingSpeedDiv;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_homingCheckRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_homingMagnitude; // Homing adjust parameter, higher values for faster turn of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_projectileSizeMult;

private:
	UPROPERTY()
		USphereComponent* m_homingCheckColl;

	UFUNCTION()
		void OnHomingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void OnStartHoming(AActor* actorToHomeTo);
};
