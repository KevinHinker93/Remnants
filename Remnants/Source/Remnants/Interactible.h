// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactible.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REMNANTS_API IInteractible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitializeInteractible();
	virtual void InteractionLogic(class ACharacter* const character);
	virtual void DeInitialize();
	
};
