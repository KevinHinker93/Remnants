// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "SpreadingShotgunComponent.generated.h"

class UWeaponFiringStrategyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpread);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API USpreadingShotgunComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
	USpreadingShotgunComponent();
	UPROPERTY(BlueprintAssignable, Category= "OnSpread")
		FOnSpread OnSpread;

	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
			TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator) override;
	virtual void EndCollision() override;
	virtual void OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing Strategies")
		TArray<TSubclassOf<UWeaponFiringStrategyBase>> m_spreadShotFiringStrategies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_sizeMult; // Decreasing size amount of spawned projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_range;

private:
	UPROPERTY()
		UWeaponFiringStrategyBase* m_spreadShotFiringStrategy;	

	bool m_hitObject;
};
