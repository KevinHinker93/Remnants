// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CapsuleCollControllerComponent.generated.h"

class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UCapsuleCollControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCapsuleCollControllerComponent();

	void SetCapsuleToControll(UCapsuleComponent* capsuleToControll);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
		UCapsuleComponent* m_capsuleToControll;

	UFUNCTION()
		void OnCrouching(APawn* crouchingCharacter, FVector crouchingHeightDiff, float crouchSpeed);
	
	float m_destHeight;
	float m_startHeight;
	float m_prevHeight;

	bool m_interpolate;
	float m_interpolationSpeed;

		
	
};
