// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementEffectComponent.h"
#include "../ElementStatusEffects/ElementStatusEffect.h"
#include "ActorStatsComponent.h"

// Sets default values for this component's properties
UElementEffectComponent::UElementEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_elementStatusEffectImpactFac = 1.0f;

	// ...
}

// If not immune to the status effect, it will be added to dictionary, but not activated. Resistance must be completely filled 
// to activate the status effect
void UElementEffectComponent::AddElementStatusEffect(ElementEffectType elementStatusEffectType, AElementStatusEffect* elementStatusEffect)
{
	m_criticalSection.Lock();
	m_elementStatusEffects.Add(elementStatusEffectType, elementStatusEffect);
	m_criticalSection.Unlock();
}

void UElementEffectComponent::RemoveElmentStatusEffect(ElementEffectType elementStatusEffectType)
{
	m_criticalSection.Lock();
	m_elementStatusEffects[elementStatusEffectType]->Destroy();
	m_elementStatusEffects.Remove(elementStatusEffectType);
	m_criticalSection.Unlock();
}

void UElementEffectComponent::RemoveAllElementStatusEffects()
{
	m_criticalSection.Lock();
	for (auto elementStatusEffectIt : m_elementStatusEffects)
	{
		elementStatusEffectIt.Value->Destroy();
	}
	m_elementStatusEffects.Empty();
	m_criticalSection.Unlock();
}

// Tells if a status effect already exists on the actor
bool UElementEffectComponent::ElementStatusEffectExists(ElementEffectType elementStatusEffectType)
{
	m_criticalSection.Lock();
	bool statusEffectExist = m_elementStatusEffects.Contains(elementStatusEffectType);
	m_criticalSection.Unlock();
	return statusEffectExist;
	
}

bool UElementEffectComponent::ElementStatusEffectIsActivated(ElementEffectType elementStatusEffectType)
{
	m_criticalSection.Lock();
	bool isActivated = m_elementStatusEffects[elementStatusEffectType]->IsActivated();
	m_criticalSection.Unlock();
	return isActivated;
}

// Returns true if resistance is completely filled
bool UElementEffectComponent::ElementStatusEffectWantsToBeActivated(ElementEffectType elementStatusEffectType)
{
	m_criticalSection.Lock();
	bool wantsToBeActivated = m_elementStatusEffects[elementStatusEffectType]->WantsToBeActivated();
	m_criticalSection.Unlock();
	return wantsToBeActivated;
}

void UElementEffectComponent::ActivateElementStatusEffect(ElementEffectType elementStatusEffectType)
{
	m_criticalSection.Lock();
	m_elementStatusEffects[elementStatusEffectType]->Activate();
	m_criticalSection.Unlock();
}

// Inflicts impact to resistance
void UElementEffectComponent::AddImpactToElementStatusEffect(ElementEffectType elementStatusEffectType, int impact)
{
	m_criticalSection.Lock();
	m_elementStatusEffects[elementStatusEffectType]->AddImpact(impact);
	m_criticalSection.Unlock();
}

void UElementEffectComponent::SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent)
{
	m_actorStatsComponent = actorStatsComponent;
}

void UElementEffectComponent::UpdateStats(bool isPlayer)
{
	if(isPlayer)
		m_ElementStatusEffectResistanceVal = m_actorStatsComponent->GetPlayerElementEffectResistanceVal();
	else
		m_ElementStatusEffectResistanceVal = m_actorStatsComponent->GetEnemyElementEffectResistanceVal();
	m_ElementStatusEffectDepleteAmount = m_actorStatsComponent->GetElementEffectDepleteAmount();
	m_ElementStatusEffectDepleteTime = m_actorStatsComponent->GetElementEffectDepleteTime();
}

int UElementEffectComponent::GetElementStatusEffectResistanceVal() const
{
	return m_ElementStatusEffectResistanceVal;
}

void UElementEffectComponent::SetElementStatusEffectResistanceVal(int elementStatusEffectResistanceVal)
{
	m_ElementStatusEffectResistanceVal = elementStatusEffectResistanceVal;
}

float UElementEffectComponent::GetElementStatusEffectDepleteAmount() const
{
	return m_ElementStatusEffectDepleteAmount;
}

float UElementEffectComponent::GetElementStatusEffectDepleteTime() const
{
	return m_ElementStatusEffectDepleteTime;
}

float UElementEffectComponent::GetElementStatusEffectImpactFac() const
{
	return m_elementStatusEffectImpactFac;
}

void UElementEffectComponent::SetElementStatusEffectImpactFac(float elementStatusEffectImpactFac)
{
	m_elementStatusEffectImpactFac = elementStatusEffectImpactFac;
}

// Called when the game starts
void UElementEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UElementEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

