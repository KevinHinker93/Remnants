// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "RangeBasedComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API URangeBasedComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator) override;
	virtual void EndCollision() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		float m_rangeDamageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Settings")
		int m_damageRate;
};
