// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterSprintingState.h"

void UCharacterSprintingState::StateBegin(APawn* stateUser)
{

}
void UCharacterSprintingState::StateEnd(APawn* stateUser)
{

}

void UCharacterSprintingState::MoveForward(APawn* pawnToMove, float val, float mult)
{
	Super::MoveForward(pawnToMove, val, mult);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (checkSingleton)
	{
		eventHandler->OnCharacterSprinting.Broadcast(pawnToMove);
	}

	//if (val != 0.0f)
	//{
	//	float moveVal = (val * m_moveSpeedMult) * mult;
	//	// add movement in that direction
	//	pawnToMove->AddMovementInput(pawnToMove->GetActorForwardVector(), moveVal);

	//	bool checkSingleton = false;
	//	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	//	if (checkSingleton)
	//	{
	//		eventHandler->OnCharacterSprinting.Broadcast(pawnToMove);
	//	}
	//}
}

void UCharacterSprintingState::Strafe(APawn* pawnToMove, float val, float mult)
{
	Super::Strafe(pawnToMove, val, mult);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (checkSingleton)
	{
		eventHandler->OnCharacterSprinting.Broadcast(pawnToMove);
	}

	//if (val != 0.0f)
	//{
	//	float moveVal = (val * m_strafeSpeedMult) * mult;
	//	// add movement in that direction
	//	pawnToMove->AddMovementInput(pawnToMove->GetActorRightVector(), moveVal);
	//	
	//	bool checkSingleton = false;
	//	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	//	if (checkSingleton)
	//	{
	//		eventHandler->OnCharacterSprinting.Broadcast(pawnToMove);
	//	}
	//}
}


