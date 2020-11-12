// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementStatusEffects/ElementStatusEffectComponentBase.h"
#include "StatsEnum.h"
#include "ManipulateStatComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UManipulateStatComponent : public UElementStatusEffectComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateComp() override;
	virtual void ShutdownComp() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manipulated Stat")
		EntityStats m_statToManipulate;

	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	
};
