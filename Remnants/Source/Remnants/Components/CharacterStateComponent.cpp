// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateComponent.h"


// Sets default values for this component's properties
UCharacterStateComponent::UCharacterStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCharacterStateComponent::SetCurrentState(CharacterStates state)
{
	if (m_currentState->GetStateType() == CharacterStates::Paralysed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot change state while paralysed"));
		return;
	}
	checkf(m_characterStates.Contains(state), TEXT("CharacterState not available, have you forgot to set it?"));

	m_previousState = m_currentState;
	APawn* owner = Cast<APawn>(GetOwner());
	m_currentState->StateEnd(owner);
	m_currentState = m_characterStates[state].GetDefaultObject();
	m_currentState->StateBegin(owner);

}

void UCharacterStateComponent::ResetCurrentState()
{
	m_previousState = m_characterStates[CharacterStates::Running].GetDefaultObject();
	APawn* owner = Cast<APawn>(GetOwner());
	m_currentState->StateEnd(owner);
	m_currentState = m_characterStates[CharacterStates::Running].GetDefaultObject();
	m_currentState->StateBegin(owner);
}

void UCharacterStateComponent::SetPreviousState(CharacterStates previousState)
{
	m_previousState = m_characterStates[previousState].GetDefaultObject();
}

UCharacterMovementState* UCharacterStateComponent::GetCurrentState() const
{
	return m_currentState;
}

UCharacterMovementState* UCharacterStateComponent::GetPreviousState() const
{
	return m_previousState;
}

// Called when the game starts
void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	m_currentState = m_characterStates[CharacterStates::Running].GetDefaultObject();
	m_previousState = m_characterStates[CharacterStates::Running].GetDefaultObject();
}


// Called every frame
void UCharacterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

