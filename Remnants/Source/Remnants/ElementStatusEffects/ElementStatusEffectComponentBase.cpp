// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementStatusEffectComponentBase.h"


// Sets default values for this component's properties
UElementStatusEffectComponentBase::UElementStatusEffectComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UElementStatusEffectComponentBase::Init(class IElementStatusEffectVictim* victim, float strength, float interval)
{
	m_victim = victim;
	m_elementStatusEffectStrength = strength;
	m_elementStatusEffectInterval = interval;
}

int UElementStatusEffectComponentBase::GetIdentifier() const
{
	return m_effectIdentifier;
}