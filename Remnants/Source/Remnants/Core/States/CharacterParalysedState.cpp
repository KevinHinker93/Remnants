// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterParalysedState.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"

void UCharacterParalysedState::StateBegin(APawn* stateUser)
{
	if(stateUser->GetController())
		stateUser->GetController()->SetIgnoreLookInput(true);	
}
void UCharacterParalysedState::StateEnd(APawn* stateUser)
{
	if(stateUser->GetController())
		stateUser->GetController()->ResetIgnoreLookInput();
}

void UCharacterParalysedState::MoveForward(APawn* pawnToMove, float val, float mult)
{
	Super::MoveForward(pawnToMove, val, mult);
}

void UCharacterParalysedState::Strafe(APawn* pawnToMove, float val, float mult)
{
	Super::Strafe(pawnToMove, val, mult);
}



