// Fill out your copyright notice in the Description page of Project Settings.

#include "CapsuleCollControllerComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values for this component's properties
UCapsuleCollControllerComponent::UCapsuleCollControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCapsuleCollControllerComponent::SetCapsuleToControll(UCapsuleComponent* capsuleToControll)
{
	m_capsuleToControll = capsuleToControll;
}

// Called when the game starts
void UCapsuleCollControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterCrouching.AddDynamic(this, &UCapsuleCollControllerComponent::OnCrouching);
	
}

void UCapsuleCollControllerComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterCrouching.RemoveDynamic(this, &UCapsuleCollControllerComponent::OnCrouching);
}

// Called every frame
void UCapsuleCollControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_interpolate)
	{
		m_prevHeight = m_startHeight;
		m_capsuleToControll->SetCapsuleHalfHeight(FMath::Lerp(m_startHeight, m_destHeight, DeltaTime * m_interpolationSpeed));
		m_startHeight = m_capsuleToControll->GetUnscaledCapsuleHalfHeight();

		if (m_startHeight == m_prevHeight)
		{
			m_interpolate = false;

			m_startHeight = 0;
			m_destHeight = 0;
		}
	}
}

void UCapsuleCollControllerComponent::OnCrouching(APawn* crouchingCharacter, FVector crouchingHeightDiff, float crouchSpeed)
{
	if (crouchingCharacter == Cast<APawn>(GetOwner()))
	{
		m_startHeight = m_capsuleToControll->GetUnscaledCapsuleHalfHeight();
		if (m_destHeight == 0.0f) // If interpolation is finished startheight is at its desteny value, so we can add crouchDiff to it
			m_destHeight = m_startHeight + crouchingHeightDiff.Z;
		else
			m_destHeight = m_destHeight + crouchingHeightDiff.Z; // If interpolation is not finished adding crouchDiff to startheight would make the character bigger

		m_interpolationSpeed = crouchSpeed;
		m_interpolate = true;
	}
}