// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "DancingProjectileComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UDancingProjectileComponent : public UProjectileBehaviourComponentBase
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
		int m_jumpCount; // How often projectile will jump to near enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_jumpRadius; // The range of jumps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_jumpMagnitude; // Homing adjust parameter, higher values for faster turn of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_dancingSpeedDivisor; // How much the projectile will slow down after starting to dance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_projectileSizeMult; // How much the projectile will slow down after starting to dance

private:
	UPROPERTY()
		AActor* m_nextTarget;

	UPROPERTY()
		TArray<AActor*> m_enemiesHit;

	bool m_startedToDance;
	float m_lifeTime;
	void FindNextTarget(AActor* lastHitActor);
	bool CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors);
};
