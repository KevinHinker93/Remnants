// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterMovementState.h"

void UCharacterMovementState::MoveForward(APawn* pawnToMove, float val, float mult)
{
	if (val != 0.0f)
	{
		float moveVal = (val * m_moveSpeedMult) * mult;
		// add movement in that direction
		pawnToMove->AddMovementInput(pawnToMove->GetActorForwardVector(), moveVal);
	}
}

void UCharacterMovementState::Strafe(APawn* pawnToMove, float val, float mult)
{
	if (val != 0.0f)
	{
		float moveVal = (val * m_strafeSpeedMult) * mult;
		// add movement in that direction
		pawnToMove->AddMovementInput(pawnToMove->GetActorRightVector(), moveVal);
	}
}

CharacterStates UCharacterMovementState::GetStateType()
{
	return m_stateType;
}