// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../ElementDamageType.h"
#include "BeamBehaviorComponentBase.generated.h"

class AShootergameBeam;
class AController;

UCLASS( Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), ABSTRACT )
class REMNANTS_API UBeamBehaviorComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBeamBehaviorComponentBase();

	virtual void InitBehaviour(float damage, AActor* damageCauser, TSubclassOf<class UElementDamageType> damageTypeClass, int moduleType);
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) { check(0 && "You must override this"); }
	virtual void OnDeath();
	virtual void OnOverlapEnd() { check(0 && "You must override this"); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_overheatingInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_damageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_overheatingMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_maxBeamRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		FVector m_initialBeamSize;

	FTimerHandle m_damageIntervalHandle;
	FTimerHandle m_overheatingIntervalHandle;
		
	UPROPERTY()
		AShootergameBeam* m_beam;
	UPROPERTY()
		AActor* m_damageCauser;
	UPROPERTY()
		TSubclassOf<class UElementDamageType> m_damageTypeClass;

	float m_beamDamage;	
	int m_firedModuleType;

	FVector m_currentBeamSize;

	UFUNCTION(Blueprintcallable, Category = "Beam")
		FVector GetCurrentBeamSize();

	bool CheckTargetIsBlocked(AActor* target, TArray<AActor*> ignoredActors);

	void OnOverheat();
};
