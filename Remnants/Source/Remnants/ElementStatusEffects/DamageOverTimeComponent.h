// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementStatusEffects/ElementStatusEffectComponentBase.h"
#include "DamageOverTimeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UDamageOverTimeComponent : public UElementStatusEffectComponentBase
{
	GENERATED_BODY()
	
public:

	virtual void ActivateComp() override;
	virtual void ShutdownComp() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class UShieldComponent* m_victimShieldComponent;

	FTimerHandle m_damageInvervalHandle;

	int m_victimMaxHealth;

	void OnDamageVictim();
	void DamageVictim();	
};
