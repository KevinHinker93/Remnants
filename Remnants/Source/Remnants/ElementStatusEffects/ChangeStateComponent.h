// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementStatusEffects/ElementStatusEffectComponentBase.h"
#include "StatsEnum.h"
#include "ChangeStateComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UChangeStateComponent : public UElementStatusEffectComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateComp() override;
	virtual void ShutdownComp() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Changed State")
		EntityState m_stateToChange;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	
};
