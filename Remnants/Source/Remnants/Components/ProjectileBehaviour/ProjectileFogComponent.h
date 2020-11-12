// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "ProjectileFogComponent.generated.h"

class ADamageArea;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UProjectileFogComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
			TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator) override;
	virtual void EndCollision() override;
	virtual void OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult) override;

	UFUNCTION(BlueprintCallable)
		void SetCanSpawnFog(bool canSpawn) { m_canSpawnFog = canSpawn; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog Settings")
		bool m_attachFogToHittedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageAreaSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageAreaDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageAreaDamagingInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		TSubclassOf<ADamageArea> m_damageArea; 

private:
	bool m_canSpawnFog;

	void SpawnFog(AActor* hittedActor, AController* eventInstigator, float lifeSpan);
	void TryAttachToActor(ADamageArea* dmgArea, AActor* actorToAttachTo);
};
