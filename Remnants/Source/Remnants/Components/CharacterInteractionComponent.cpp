// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterInteractionComponent.h"
#include "../AInteractible.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "../AInteractible.h"


// Sets default values for this component's properties
UCharacterInteractionComponent::UCharacterInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCharacterInteractionComponent::Interact()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (m_interactibleObject && character)
		m_interactibleObject->InteractionLogic(character);
}

void UCharacterInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnEnterInteractible.AddDynamic(this, &UCharacterInteractionComponent::InitializeInteractible);
	eventHandler->OnExitInteractible.AddDynamic(this, &UCharacterInteractionComponent::DeInitializeInteractible);	
}

void UCharacterInteractionComponent::EndPlay(EEndPlayReason::Type type)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnEnterInteractible.RemoveDynamic(this, &UCharacterInteractionComponent::InitializeInteractible);
	eventHandler->OnExitInteractible.RemoveDynamic(this, &UCharacterInteractionComponent::DeInitializeInteractible);	
}

void UCharacterInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterInteractionComponent::InitializeInteractible(AAInteractible* interactible)
{
	if (interactible)
	{
		m_interactibleObject = interactible;
		UE_LOG(LogTemp, Display, TEXT("Player interactible set: %s"), *interactible->GetName());
		m_interactibleObject->InitializeInteractible();
	}
}

void UCharacterInteractionComponent::DeInitializeInteractible(AAInteractible* interactible)
{
	if (m_interactibleObject && m_interactibleObject == interactible)
	{
		UE_LOG(LogTemp, Display, TEXT("Player interactible removed: %s"), *interactible->GetName());
		m_interactibleObject->DeInitialize();
		m_interactibleObject = nullptr;
	}
}