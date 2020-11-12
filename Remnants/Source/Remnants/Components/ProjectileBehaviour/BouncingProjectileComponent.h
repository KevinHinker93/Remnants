// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/DefaultProjectileBehaviourComp.h"
#include "BouncingProjectileComponent.generated.h"

class USoundBase;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UBouncingProjectileComponent : public UDefaultProjectileBehaviourComp
{
	GENERATED_BODY()
	
public:
	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner,
		TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		float m_projectileSizeMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		float m_damageMultPerBounce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		float m_bounceLifetimeExtendMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		int m_maxBounces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
		USoundBase* m_bounceSound;

private:
	int m_currentBounceCount;

	void Bounce(const FVector& hitLoc);
	
};
