// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterCrouchingState.h"

void UCharacterCrouchingState::StateBegin(APawn* stateUser)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (checkSingleton)
	{
		eventHandler->OnCharacterCrouching.Broadcast(stateUser, FVector(0,0, -m_crouchingHeightDiff), m_crouchSpeed);
	}
}
void UCharacterCrouchingState::StateEnd(APawn* stateUser)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (checkSingleton)
	{
		eventHandler->OnCharacterCrouching.Broadcast(stateUser, FVector(0, 0, m_crouchingHeightDiff), m_crouchSpeed);
	}
}

void UCharacterCrouchingState::MoveForward(APawn* pawnToMove, float val, float mult)
{
	Super::MoveForward(pawnToMove, val, mult);
	//if (val != 0.0f)
	//{
	//	float moveVal = (val * m_moveSpeedMult) * mult;
	//	// add movement in that direction
	//	pawnToMove->AddMovementInput(pawnToMove->GetActorForwardVector(), moveVal);
	//}
}

void UCharacterCrouchingState::Strafe(APawn* pawnToMove, float val, float mult)
{
	Super::Strafe(pawnToMove, val, mult);
	//if (val != 0.0f)
	//{
	//	float moveVal = (val * m_strafeSpeedMult) * mult;
	//	// add movement in that direction
	//	pawnToMove->AddMovementInput(pawnToMove->GetActorRightVector(), moveVal);
	//}
}



