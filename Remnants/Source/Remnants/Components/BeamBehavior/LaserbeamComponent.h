// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BeamBehavior/BeamBehaviorComponentBase.h"
#include "TimerManager.h"
#include "LaserbeamComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API ULaserbeamComponent : public UBeamBehaviorComponentBase
{
	GENERATED_BODY()

public:
	virtual void InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) override;
	virtual void OnDeath() override;
	virtual void OnOverlapEnd() override;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_timeToDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_beamLifeTime;

private:
	FTimerHandle m_timeToDamageHandle;
	FTimerDelegate m_timerDelegate;

	UFUNCTION()
		virtual void OnDamageEnemy(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
				AController* EventInstigator);
};
