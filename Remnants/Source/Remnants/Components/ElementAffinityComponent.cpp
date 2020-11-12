// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementAffinityComponent.h"
#include "../Core/Items/AElement.h"
#include "../ElementDamageType.h"
#include "../EventSystemHandlerSingleton.h"

UElementAffinityComponent::UElementAffinityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_affinityInitialized = false;
}

void UElementAffinityComponent::RequestElementAffinity(ElementEffectType type)
{
	if (m_affinityInitialized)
		return;

	if (m_affinityList.Contains(type))
	{
		TArray<FElementAffinityData> affinityDatas = m_affinityList[type].m_affinities;
		TArray<FItemEffectivenessData> itemIds;
		for (int i = 0; i < affinityDatas.Num(); i++)
		{
			FElementAffinityData affinityData = affinityDatas[i];
			float damageMult = affinityData.m_damageMult;
			if (!m_elementAffinities.Contains(affinityData.m_type))
				m_elementAffinities.Add(affinityData.m_type, damageMult);
			if (m_elementItemIds.Contains(affinityData.m_type))
			{
				EDamageEffectiveness damageEffectiveness = CalcEffectivenessFromDamageMult(damageMult);
				FItemEffectivenessData itemData;
				itemData.m_itemId = m_elementItemIds[affinityData.m_type];
				itemData.m_effectiveness = damageEffectiveness;
				itemIds.Add(itemData);
			}
		}
		bool isValid = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(isValid);
		if (isValid)
			eventHandler->OnSetElementAffinity.Broadcast(GetOwner(), itemIds);
		m_affinityInitialized = true;
	}
}

EDamageEffectiveness UElementAffinityComponent::GetElementEffectiveness(ElementEffectType type, float& damageMult)
{
	EDamageEffectiveness effectiveness = EDamageEffectiveness::Normal;
	float tempDamageMult = 1.0f;
	if (m_elementAffinities.Contains(type))
	{
		tempDamageMult = m_elementAffinities[type];
		effectiveness = CalcEffectivenessFromDamageMult(tempDamageMult);
	}
	
	damageMult = tempDamageMult;
	return effectiveness;
}

TArray<int> UElementAffinityComponent::GetElementWeaknesses()
{
	TArray<int> effs{};
	for (auto kvp : m_elementAffinities)
	{
		if (!m_elementItemIds.Contains(kvp.Key))
		{
			UE_LOG(LogTemp, Error, TEXT("Element Effectiveness Item ID could not be found"));
			return effs;
		}
		if (GetElementEffectiveness(kvp.Key, kvp.Value) == EDamageEffectiveness::Effective)
		{
			int itemId = m_elementItemIds[kvp.Key];
			effs.Add(itemId);
		}
	}
	return effs;
}

void UElementAffinityComponent::BeginPlay()
{
	Super::BeginPlay();

	bool isValid = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(isValid);
	if (isValid)
		eventHandler->OnReceivedElement.AddDynamic(this, &UElementAffinityComponent::OnActorReceivedElement);
}

void UElementAffinityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UElementAffinityComponent::OnActorReceivedElement(AActor* actorReceivedElement, UAElement* element)
{
	AActor* owner = GetOwner();
	if (!owner || owner->IsPendingKill())
		return;
	if (owner != actorReceivedElement)
		return;
	
	TSubclassOf<UElementDamageType> damageType = element->GetDamageTypeClass();
	if (!damageType)
		return;

	ElementEffectType type = damageType.GetDefaultObject()->GetElementType();

	RequestElementAffinity(type);
}

EDamageEffectiveness UElementAffinityComponent::CalcEffectivenessFromDamageMult(float damageMult)
{
	EDamageEffectiveness effectiveness = EDamageEffectiveness::Normal;

	if (damageMult > 1.0f)
		effectiveness = EDamageEffectiveness::Effective;
	else if (damageMult < 1.0f)
		effectiveness = EDamageEffectiveness::UnEffective;

	return effectiveness;
}