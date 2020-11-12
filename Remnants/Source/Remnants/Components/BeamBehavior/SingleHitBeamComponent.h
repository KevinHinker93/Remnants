// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BeamBehavior/BeamBehaviorComponentBase.h"
#include "TimerManager.h"
#include "SingleHitBeamComponent.generated.h"

class AController;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API USingleHitBeamComponent : public UBeamBehaviorComponentBase
{
	GENERATED_BODY()

public:
	virtual void InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType) override;
	/*virtual void OnHit(AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass);*/
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) override;
	virtual void OnDeath() override;
	virtual void OnOverlapEnd() override;

	virtual void OnApplyDamage(AActor* damagedActor, float baseDamage, FVector const& hitFromDirection, FHitResult const& hitInfo,
			AController* eventInstigator, AActor* damageCauser, TSubclassOf<UDamageType> damageTypeClass);

	// Time when beam starts to lose energy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_loseEnergyTime;
	// Time beam completely loses all energy in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_loseEnergyDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_energyLossAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		bool m_showBeamExplosion;

	FTimerDelegate m_timerDelegate;
	
private:
	FTimerHandle m_beamLoseEnergyTimerHandle;

	float m_loseEnergyInterval;

	UFUNCTION()
		virtual void OnDamageEnemy(AActor* damagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
				AController* EventInstigator);

	void OnLoseEnergy();
};
