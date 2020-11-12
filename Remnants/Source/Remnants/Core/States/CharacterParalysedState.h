// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMovementState.h"
#include "CharacterParalysedState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REMNANTS_API UCharacterParalysedState : public UCharacterMovementState
{
	GENERATED_BODY()
	
public:
	void StateBegin(APawn* stateUser) override;
	void StateEnd(APawn* stateUser) override;
	void MoveForward(APawn* pawnToMove, float val, float mult) override;
	void Strafe(APawn* pawnToMove, float val, float mult) override;	
};
