// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMovementState.h"
#include "CharacterRunningState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REMNANTS_API UCharacterRunningState : public UCharacterMovementState
{
	GENERATED_BODY()
	
public:
	void StateBegin(APawn* stateUser) override;
	void StateEnd(APawn* stateUser) override;
	void MoveForward(APawn* pawnToMove, float val, float mult) override;
	void Strafe(APawn* pawnToMove, float val, float mult) override;
};
