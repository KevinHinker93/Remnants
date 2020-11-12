// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BeamBehavior/SingleHitBeamComponent.h"
#include "../../WeaponFiringStrategies/ShotgunStrategy.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "PullingBeamComponent.generated.h"

class AAEnemyDrone;
class UElementDamageType;
class UCameraComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPullEnemy, FVector, start, FVector, end, AActor*, actorToPull);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UPullingBeamComponent : public USingleHitBeamComponent
{
	GENERATED_BODY()
	
public:
	// Pulls enemy from a given start point to an end point
	UPROPERTY(BlueprintAssignable, Category= "PullEnemy")
		FOnPullEnemy PullEnemy;

	// Updates pull direction
	UFUNCTION(BlueprintCallable, Category="PullEnemy")
		void UpdatePullDirection();

	UFUNCTION(BlueprintCallable, Category = "PullEnemy")
		void AbsorbProjectile(AShootergameProjectile* projectile);

	virtual void InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<UElementDamageType> damageTypeClass, int moduleType) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) override;
	virtual void OnDeath() override;
	virtual void OnOverlapEnd() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		TSubclassOf<UShotgunStrategy> m_counterShotgunFiringStrat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		TSubclassOf<AShootergameProjectile> m_projectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element Sound")
		USoundBase* m_counterProjSound;
	// How much impact an absorbed projectile has on raw beam damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		float m_absorbedProjectileDamageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		float m_counterProjectileDamageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		float m_counterRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		float m_counterProjectileSizeMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		int m_maxProjectiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		float m_firedConeOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_pullStrength;
	// Minimum pulling value to pull enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_pullThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_pullingInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_defaultPullMass;

private:
	FTimerHandle m_pullTimerHandle;
	// Camera of player for getting forward vector of player
	UPROPERTY()
		UCameraComponent* m_playerCamera;
	UPROPERTY()
		AAEnemyDrone* m_enemyToPull;
	UPROPERTY()
		AActor* m_actorToPull;

	int m_absorbedProjectileCount;
	float m_initialDamageMult;
	float accumulatedAbsorbedDamageFromProjectiles;
	bool m_died;

	float CalcDesiredPullStrength();

	UFUNCTION()
		void OnPullEnemy(AActor* actorToPull);

	void ResetPulledObjects();
};
