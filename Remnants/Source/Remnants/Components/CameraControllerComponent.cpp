// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraControllerComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UCameraControllerComponent::UCameraControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCameraControllerComponent::SetCameraToControll(UCameraComponent* cameraToControll)
{
	m_cameraToControll = cameraToControll;
}

// Called when the game starts
void UCameraControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterCrouching.AddDynamic(this, &UCameraControllerComponent::OnCrouching);
	
}

void UCameraControllerComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterCrouching.RemoveDynamic(this, &UCameraControllerComponent::OnCrouching);
}

// Called every frame
void UCameraControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCameraControllerComponent::OnCrouching(APawn* crouchingCharacter, FVector crouchingHeightDiff, float crouchSpeed)
{
	if (crouchingCharacter == Cast<APawn>(GetOwner()))
	{
	}
}
