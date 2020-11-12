// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BeamBehavior/BeamBehaviorComponentBase.h"
#include "WideAreaBeamComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UWideAreaBeamComponent : public UBeamBehaviorComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType) override;
	/*virtual void OnHit(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass);*/
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) override;
	virtual void OnDeath() override;
	void OnOverlapEnd() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		FName m_desiredImpactState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		bool m_hasImpactOnUserMoveSpeed;

private:
	void DamageAllEnemiesInside();
};
