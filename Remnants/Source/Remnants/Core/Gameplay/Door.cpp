// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Components/BoxComponent.h"
#include "EventSystemHandlerSingleton.h"
#include "Components/SceneComponent.h"
#include "../../Core/Characters/ShootergameCharacter.h"
#include "Components/InventoryComponent.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_currentState = DoorStates::Closed;

	m_canBeClosed = true;
	m_locked = false;
	m_lockAfterEntered = false;
	m_canInteract = true;
	m_interactionTexts.SetNum(2);
}

void ADoor::InitializeInteractible()
{
	Super::InitializeInteractible();
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FText interactionText = FText::FromName(m_interactionTexts[int(m_currentState)]);
	eventHandler->OnModifyInteractionWidget.Broadcast(true, true, interactionText);
}

void ADoor::InteractionLogic(ACharacter* const character)
{
	UE_LOG(LogTemp, Display, TEXT("Player opened or closed door"));
		
	if (m_lockable && m_locked && m_requiredItemIdToUnlock >= 0)
	{
		AShootergameCharacter* player = Cast<AShootergameCharacter>(character);
		if (player)
			TryUnlockDoor(player);
	}
	else
	{
		if (m_currentState == DoorStates::Closed)
			OpenDoor();
		else
			CloseDoor();
	}
}

void ADoor::DeInitialize()
{
	Super::DeInitialize();
	UE_LOG(LogTemp, Display, TEXT("Door interactible deinitialized"));
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FName interactionText = TEXT("");
	eventHandler->OnModifyInteractionWidget.Broadcast(true, false, FText::FromName(interactionText));
}

void ADoor::TryUnlockDoor(AShootergameCharacter* player)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (m_lockable && m_currentState == DoorStates::Closed)
	{
		eventHandler->OnCharacterHasItemResponse.AddDynamic(this, &ADoor::OnCharacterHasItemResponse);
		eventHandler->OnCharacterHasItemRequest.Broadcast(this, player, m_requiredItemIdToUnlock);
	}
	else
	{
		eventHandler->OnDisplayUIMessage.Broadcast(TEXT("Door is locked"), -1);
	}
}

void ADoor::OnPedestalInsertionDelegate(FName eventName)
{
	if (m_lockable && eventName == m_eventNameToUnlock)
		SetLocked(false);	
}

void ADoor::OnRequestOpenDoorDelegate(FName eventName)
{
	if (m_lockable && eventName == m_eventNameToUnlock)
		SetLocked(false);
}

void ADoor::SetLocked(bool locked)
{
	if (!m_lockable) return;

	m_locked = locked;
	if (!m_locked)
		OnDoorUnlocked();
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnDoorStateChanged.Broadcast(GetName(), locked);
}

void ADoor::EndPlay(EEndPlayReason::Type type)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPedestalInsertionDelegate.RemoveDynamic(this, &ADoor::OnPedestalInsertionDelegate);
	eventHandler->OnDoorStateChangeRequested.RemoveDynamic(this, &ADoor::OnStateChangeRequested);
	eventHandler->OnCharacterHasItemResponse.RemoveDynamic(this, &ADoor::OnCharacterHasItemResponse);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPedestalInsertionDelegate.AddDynamic(this, &ADoor::OnPedestalInsertionDelegate);
	eventHandler->OnDoorStateChangeRequested.AddDynamic(this, &ADoor::OnStateChangeRequested);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::OpenDoor()
{
	if (!m_locked)
	{
		m_canInteract = false;
		m_currentState = DoorStates::Open;
		AnimateDoor(int(m_currentState));
	}
	else
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnDisplayUIMessage.Broadcast(TEXT("Door is locked"), -1);
	}
}

void ADoor::CloseDoor()
{
	if (m_canBeClosed)
	{
		m_canInteract = false;
		m_currentState = DoorStates::Closed;
		AnimateDoor(int(m_currentState));
	}
}

void ADoor::OnStateChangeRequested(const FString& doorName, bool locked)
{
	if (m_lockable && GetName() == doorName)
	{
		m_locked = locked;
		if (!m_locked)
			OnDoorUnlocked();
		UE_LOG(LogTemp, Display, TEXT("Door %s lock was set to: %s"), *doorName, locked ? TEXT("locked") : TEXT("unlocked"));
	}
}

void ADoor::OnCharacterHasItemResponse(AActor* receiver, bool hasItem)
{
	if (receiver == this && hasItem)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		SetLocked(false);
		OnUnlockDoor();
		// Display ui message that item was inserted
		FString insertionDisplayText = "Inserted ";
		FString itemName = eventHandler->GetEventFacade()->GetItemInfo(m_requiredItemIdToUnlock).m_name.ToString();
		insertionDisplayText.Append(itemName);
		eventHandler->OnDisplayUIMessage.Broadcast(*insertionDisplayText, -1);

		OpenDoor();

		eventHandler->OnCharacterHasItemResponse.RemoveDynamic(this, &ADoor::OnCharacterHasItemResponse);

	}
}
