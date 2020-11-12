// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementDamageType.h"
#include "ElementStatusEffects/ElementStatusEffect.h"

TSubclassOf<AElementStatusEffect> UElementDamageType::GetStatusEffectClass() const
{
	return m_ElementStatusEffect;
}

FName UElementDamageType::GetElementTypeName() const
{
	return m_elementTypeName;
}

ElementEffectType UElementDamageType::GetElementType() const
{
	return m_elementType;
}

int UElementDamageType::GetElementLevel() const
{
	return m_elementLevel;
}

// How much elemental impact from inflicted damage will be added
float UElementDamageType::GetElementStatusEffectImpactMult() const
{
	return m_elementStatusEffectImpactMult;
}

// Every element has a base effect value, that will be added to element resistance, when beeing hit
float UElementDamageType::GetElementStatusEffectBaseImpact() const
{
	return m_elementStatusEffectBaseImpact;
}

// Returns influence of the effect, meaning how much a stat will be reduced e.g.
TArray<float> UElementDamageType::GetElementStatusEffectStrenghts() const
{
	return m_elementStatusEffectStrenghts;
}

float UElementDamageType::GetElementStatusEffectInterval() const
{
	return m_elementStatusEffectInterval;
}

FRandomDamageMultRange UElementDamageType::GetRandomDamageMultRange()
{
	return m_randomDamageMultRange;
}
