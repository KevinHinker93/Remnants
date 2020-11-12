// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterRunningState.h"
#include "EventSystemHandlerSingleton.h"

void UCharacterRunningState::StateBegin(APawn* stateUser)
{

}
void UCharacterRunningState::StateEnd(APawn* stateUser)
{

}

void UCharacterRunningState::MoveForward(APawn* pawnToMove, float val, float mult)
{
	Super::MoveForward(pawnToMove, val, mult);
}

void UCharacterRunningState::Strafe(APawn* pawnToMove, float val, float mult)
{
	Super::Strafe(pawnToMove, val, mult);
}


