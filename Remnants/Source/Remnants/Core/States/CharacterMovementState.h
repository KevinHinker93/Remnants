// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../EventSystemHandlerSingleton.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "CharacterMovementState.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class CharacterStates : uint8
{
	Running 	UMETA(DisplayName = "Running"),
	Sprinting 	UMETA(DisplayName = "Sprinting"),
	Crouching   UMETA(DisplayName = "Crouching"),
	Paralysed   UMETA(DisplayName = "Paralysed"),
	HeavyWeapon   UMETA(DisplayName = "HeavyWeapon"),
};

/**
 * 
 */
UCLASS(ABSTRACT)
class REMNANTS_API UCharacterMovementState : public UObject
{
	GENERATED_BODY()

public:
	virtual void StateBegin(APawn* stateUser) { check(0 && "You must override this"); }
	virtual void StateEnd(APawn* stateUser) { check(0 && "You must override this"); }
	virtual void MoveForward(APawn* pawnToMove, float val, float mult);
	virtual void Strafe(APawn* pawnToMove, float val, float mult);

	CharacterStates GetStateType();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Type")
		CharacterStates m_stateType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") // General move speed
		float m_moveSpeedMult;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_strafeSpeedMult;
};
