// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "LifeLeechShotComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API ULifeLeechShotComponent : public UProjectileBehaviourComponentBase
{
	GENERATED_BODY()

public:
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator) override;
	virtual void EndCollision() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_leechPercentage;
};
