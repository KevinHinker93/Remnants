// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerUpComponent.h"
#include "ElementEffectComponent.h"
#include "../Core/Gameplay/Buff.h"
#include "../EventSystemHandlerSingleton.h"

// Sets default values for this component's properties
UPowerUpComponent::UPowerUpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	m_firedNeededPickUpCountEvent = false;
}


void UPowerUpComponent::AddPowerUpPickup()
{
	m_criticalSection.Lock();
	if (m_currentPowerUpPickUps + 1 > m_maxPowerUpPickUps)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Already collected all powerUp pickups // max: %d"), m_maxPowerUpPickUps);
		return;
	}
	m_currentPowerUpPickUps++;
	OnPowerUpCountChanged(m_currentPowerUpPickUps);

	if (m_buffsAtPickUpCount.Contains(m_currentPowerUpPickUps))
		ActivateBuff(true);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnReceivedBuffOrb.Broadcast(m_currentPowerUpPickUps);

	m_criticalSection.Unlock();
}

void UPowerUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Fetch the maximum number of power Up pickups that can be picked up
	{
		TArray<int> buffKeys;
		m_buffsAtPickUpCount.GenerateKeyArray(buffKeys);
		buffKeys.Sort();
		m_maxPowerUpPickUps = buffKeys[m_buffCount - 1];

		for (int i = 0; i < buffKeys.Num(); i++)
			OnPowerUpMaxCountChanged(i, buffKeys[i]);
	}

	OnPowerUpCountChanged(m_currentPowerUpPickUps);

	ActivateBuff(false);
}


void UPowerUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!m_firedNeededPickUpCountEvent)
	{
		TArray<int> buffKeys;
		m_buffsAtPickUpCount.GenerateKeyArray(buffKeys);
		buffKeys.Sort();

		for (int i = 0; i < buffKeys.Num(); i++)
			OnPowerUpMaxCountChanged(i, buffKeys[i]);

		m_firedNeededPickUpCountEvent = true;
	}
}

void UPowerUpComponent::ActivateBuff(bool notification)
{
	TArray<int> buffKeys;
	m_buffsAtPickUpCount.GenerateKeyArray(buffKeys);

	for (auto buffKey : buffKeys)
	{
		if (m_currentPowerUpPickUps >= buffKey)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Activated buff at: curr: %d // max: %d"), buffKey, m_maxPowerUpPickUps);
			UBuff* buffObject = m_buffsAtPickUpCount[buffKey].GetDefaultObject();
			buffObject->ActivateBuff(GetOwner());

			OnPowerUpBuffActivated(buffObject->GetBuffName(), notification);
		}
	}
}