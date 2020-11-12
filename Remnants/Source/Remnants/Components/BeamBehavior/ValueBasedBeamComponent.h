// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BeamBehavior/SingleHitBeamComponent.h"
#include "ValueBasedBeamComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UValueBasedBeamComponent : public USingleHitBeamComponent
{
	GENERATED_BODY()

public:
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_minDamageMultVal;

	virtual void OnApplyDamage(AActor* damagedActor, float baseDamage, FVector const& hitFromDirection, FHitResult const& hitInfo,
		AController* eventInstigator, AActor* damageCauser, TSubclassOf<UDamageType> damageTypeClass);
};
