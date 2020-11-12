// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/States/CharacterMovementState.h"
#include "CharacterStateComponent.generated.h"

class UCharacterMovementState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStateComponent();
	void SetCurrentState(CharacterStates state);
	void ResetCurrentState();
	void SetPreviousState(CharacterStates previousState);
	UCharacterMovementState* GetCurrentState() const;
	UCharacterMovementState* GetPreviousState() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character States", meta = (AllowPrivateAccess = "true"))
		TMap<CharacterStates, TSubclassOf<UCharacterMovementState>> m_characterStates;
	
	UPROPERTY()
		UCharacterMovementState* m_currentState;
	UPROPERTY()
		UCharacterMovementState* m_previousState; // Previous state enables good transition for example from sprinting to crouching again when crouch button is still pressed while sprinting

};
