// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraControllerComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UCameraControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraControllerComponent();
	void SetCameraToControll(UCameraComponent* cameraToControll);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
		void OnCrouching(APawn* crouchingCharacter, FVector crouchingHeightDiff, float crouchSpeed);
	
	UCameraComponent* m_cameraToControll;
	FVector m_destVector;
	FVector m_startVector;
	FVector m_prevVector;

	bool m_interpolate;
		
	
};
