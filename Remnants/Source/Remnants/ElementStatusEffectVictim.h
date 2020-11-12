// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementStatusEffects/ElementEffectTypes.h"
#include "ElementStatusEffects/StatsEnum.h"
#include "ElementStatusEffects/ElementEffectTypes.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "ElementStatusEffectVictim.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UElementStatusEffectVictim : public UInterface
{
	GENERATED_BODY()

	UElementStatusEffectVictim(const class FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
class REMNANTS_API IElementStatusEffectVictim
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Victim calls this function to remove a status effect from the status effect list and destroy the status effect actor.
	// This can happen whether the impact of the effect falls to zero or an activated status effects is over. 
	virtual void RemoveStatusEffect(ElementEffectType elementStatusEffectType);
	virtual int GetVictimMaxHealth();
	// Retrieves the victims status effect resistance
	virtual int GetVictimMaxElementImpactVal();
	virtual float GetVictimElementStatusEffectDepleteAmount();
	virtual float GetVictimElementStatusEffectDepleteTime();

	virtual class UShieldComponent* GetVictimShieldComponent();

	virtual void SetFloatVal(EntityStats stat, float val);
	virtual void ResetFloatVal(EntityStats stat, float val);

	virtual void ChangeState(EntityState state);
	virtual void ResetState(EntityState state);

	virtual void BanAction(EntityAction action);
	virtual void UnBanAction(EntityAction action);

	virtual bool IsFiring();
};
