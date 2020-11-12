// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementMachine.h"
#include "EventSystemHandlerSingleton.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
AElementMachine::AElementMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_canInteract = true;
}

void AElementMachine::InitializeInteractible()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FText interactionText = FText::FromName(m_interactionText);
	eventHandler->OnModifyInteractionWidget.Broadcast(true, true, interactionText);
}

void AElementMachine::InteractionLogic(ACharacter* const character)
{
	if (m_canInteract)
	{
		UE_LOG(LogTemp, Warning, TEXT("Toggle Element Shield"));
		SwitchBarrier();
		m_canInteract = false;
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnToggleElementClouds.Broadcast(m_elementCloudIdentifier);

		GetWorld()->GetTimerManager().SetTimer(m_interactionTimerHandle, this, &AElementMachine::EnableInteraction, m_interactionInterval, false);
	}
}

void AElementMachine::DeInitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Machine rem"));
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FText interactionText = FText::FromName(TEXT(""));
	eventHandler->OnModifyInteractionWidget.Broadcast(true, false, interactionText);
}

void AElementMachine::BeginPlay()
{
	Super::BeginPlay();
	
}

void AElementMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AElementMachine::EnableInteraction()
{
	GetWorld()->GetTimerManager().ClearTimer(m_interactionTimerHandle);
	m_canInteract = true;
}