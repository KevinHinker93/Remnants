// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementStatusEffectVictim.h"


// Add default functionality here for any IElementStatusEffectVictim functions that are not pure virtual.

UElementStatusEffectVictim::UElementStatusEffectVictim(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Victim calls this function to remove a status effect from the status effect list and destroy the status effect actor.
// This can happen whether the impact of the effect falls to zero or an activated status effects is over. 
void IElementStatusEffectVictim::RemoveStatusEffect(ElementEffectType elementStatusEffectType)
{

}

// Retrieves the victims status effect resistance
int IElementStatusEffectVictim::GetVictimMaxElementImpactVal()
{
	return 0;
}

float IElementStatusEffectVictim::GetVictimElementStatusEffectDepleteAmount()
{
	return 0;
}
float IElementStatusEffectVictim::GetVictimElementStatusEffectDepleteTime()
{
	return 0;
}

int IElementStatusEffectVictim::GetVictimMaxHealth()
{
	return 0;
}

class UShieldComponent* IElementStatusEffectVictim::GetVictimShieldComponent()
{
	return nullptr;
}

void IElementStatusEffectVictim::SetFloatVal(EntityStats stat, float val)
{

}

void IElementStatusEffectVictim::ResetFloatVal(EntityStats stat, float val)
{

}

void IElementStatusEffectVictim::ChangeState(EntityState state)
{

}

void IElementStatusEffectVictim::ResetState(EntityState state)
{

}

void IElementStatusEffectVictim::BanAction(EntityAction action)
{

}

void IElementStatusEffectVictim::UnBanAction(EntityAction action)
{

}

bool IElementStatusEffectVictim::IsFiring()
{
	return false;
}