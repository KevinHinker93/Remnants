// Fill out your copyright notice in the Description page of Project Settings.

#include "StaminaComponent.h"
#include "../EventSystemHandlerSingleton.h"


// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_maxStamina = 100.0f;
	m_staminaCost = 20.0f;
	m_staminaRegen = 20.0f;
	m_fitThreshold = 0.35f;
	m_currentStamina = m_maxStamina;
	m_exhaustedRegenMult = 0.65f;
}

bool UStaminaComponent::IsExhausted()
{
	return m_staminaState == StaminaStates::Exhausted;
}

// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	m_staminaState = StaminaStates::Fit;
	m_currentStamina = m_maxStamina;
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterSprinting.AddDynamic(this, &UStaminaComponent::OnSprinting);
	eventHandler->OnCharacterStopSprinting.AddDynamic(this, &UStaminaComponent::OnStopSprinting);
}

void UStaminaComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterSprinting.RemoveDynamic(this, &UStaminaComponent::OnSprinting);
	eventHandler->OnCharacterStopSprinting.RemoveDynamic(this, &UStaminaComponent::OnStopSprinting);
}

// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_staminaState == StaminaStates::Fit)
	{
		if (m_currentStamina < m_maxStamina)
			m_currentStamina = FMath::Max(0.0f, FMath::Min(m_currentStamina + (m_staminaRegen * DeltaTime), m_maxStamina));
	}
	else if (m_staminaState == StaminaStates::Deplete)
	{
		m_currentStamina = FMath::Max(0.0f, FMath::Min(m_currentStamina - (m_staminaCost * DeltaTime), m_maxStamina));
		if (m_currentStamina == 0)
		{
			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

			if (checkSingleton)
				eventHandler->OnCharacterStopSprinting.Broadcast(Cast<APawn>(GetOwner()));
		}
		else
			m_staminaState = StaminaStates::Fit;
	}
	else // Exhausted:
	{
		if (m_currentStamina < m_maxStamina)
			m_currentStamina = FMath::Max(0.0f, FMath::Min(m_currentStamina + ((m_staminaRegen * m_exhaustedRegenMult) * DeltaTime), m_maxStamina));

		if (m_currentStamina >= (m_maxStamina * m_fitThreshold))
			m_staminaState = StaminaStates::Fit;
	}
}

void UStaminaComponent::OnSprinting(APawn* sprintingPawn)
{
	if (sprintingPawn == Cast<APawn>(GetOwner()))
	{
		m_staminaState = StaminaStates::Deplete;
	}
}

void UStaminaComponent::OnStopSprinting(APawn* sprintingPawn)
{
	if (sprintingPawn == Cast<APawn>(GetOwner()))
	{
		if (m_currentStamina < (m_maxStamina * m_fitThreshold))
			m_staminaState = StaminaStates::Exhausted;
		else
			m_staminaState = StaminaStates::Fit;
	}
}

