// Fill out your copyright notice in the Description page of Project Settings.

#include "Pedestal.h"
#include "../../EventSystemHandlerSingleton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"

// Sets default values
APedestal::APedestal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void APedestal::InitializeInteractible()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	FString interactionDisplayText = "Insert ";
	FString currentWantedItem = m_currentElementSlotName.ToString();
	interactionDisplayText.Append(currentWantedItem.RightChop(3));
	interactionDisplayText.Append(" Key");
	FText interactionText = FText::FromString(interactionDisplayText);
	eventHandler->OnModifyInteractionWidget.Broadcast(true, true, interactionText);
}

void APedestal::InteractionLogic(ACharacter* const character)
{
	AActor* receiver = Cast<AActor>(character);
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	if (m_requestedItemName == "")
	{
		m_requestedItemName = m_currentElementSlotName;
		eventHandler->OnCharacterHasItemRequest.Broadcast(this, receiver, m_requiredKeyItems[m_requestedItemName]);
	}
}

void APedestal::DeInitialize()
{
	UE_LOG(LogTemp, Display, TEXT("Pedestal interactible deinitialized"));
	m_currentElementSlotName = "";
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnModifyInteractionWidget.Broadcast(true, false, FText::FromName(m_currentElementSlotName));
}

void APedestal::OnCharacterHasItemResponse(AActor* receivingCharacter, bool hasItem)
{
	if (receivingCharacter == this)
	{
		if (!hasItem)
		{
			UE_LOG(LogTemp, Display, TEXT("Pedestal: Player doesn't have item %s"), *m_requestedItemName.ToString());
			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
			FString uiDisplayText = "No ";
			FString currentWantedItem = m_requestedItemName.ToString();
			uiDisplayText.Append(currentWantedItem.RightChop(3));
			uiDisplayText.Append(" Key in inventory");
			eventHandler->OnDisplayUIMessage.Broadcast(*uiDisplayText, -1);
			m_requestedItemName = "";
			return;
		}

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		if (m_insertedItemIDs.Num() == 0) // means we're inserting base key
		{
			eventHandler->OnPedestalInsertionDelegate.Broadcast(TEXT("BaseKey"));
			AnimatePedestal();
		}

		int itemId = m_requiredKeyItems[m_requestedItemName];
		
		if (m_insertedItemIDs.Contains(itemId))
		{
			UE_LOG(LogTemp, Warning, TEXT("Pedestal already contains item with ID %d"), itemId);
			m_requestedItemName = "";
			return;
		}

		// Display ui message that item was inserted
		{
			checkSingleton = false;
			FString insertionDisplayText = "Inserted ";
			FString currentWantedItem = m_requestedItemName.ToString();
			insertionDisplayText.Append(currentWantedItem.RightChop(3));
			insertionDisplayText.Append(" Key");
			eventHandler->OnDisplayUIMessage.Broadcast(*insertionDisplayText, -1);
		}

		KeyInserted();

		m_insertedItemIDs.Add(itemId);

		if (m_insertedItemIDs.Num() >= m_requiredKeyItems.Num())
		{
			eventHandler->OnPedestalInsertionDelegate.Broadcast(TEXT("AllKeys"));
		}

		ApplyVisibility();
		eventHandler->OnPedestalUpdated.Broadcast(m_insertedItemIDs);
		// save both player and level
		eventHandler->OnSaveRequested.Broadcast();
	}

	m_requestedItemName = "";
}

void APedestal::BeginPlay()
{
	Super::BeginPlay();

	// put meshes into map for easier access later---------------------------
	TArray<UActorComponent*> staticMeshes = GetComponentsByClass(UStaticMeshComponent::StaticClass());
	TArray<UActorComponent*> interactionAreas = GetComponentsByClass(UBoxComponent::StaticClass());
	for (auto kvp : m_requiredKeyItems)
	{
		FName name = kvp.Key;

		for (auto staticMesh : staticMeshes)
			if (staticMesh->GetName() == name.ToString())
				m_itemMeshes.Emplace(kvp.Value, Cast<UStaticMeshComponent>(staticMesh));

		for (auto interactionArea : interactionAreas)
		{
			if (interactionArea->ComponentHasTag("KeyBase"))
			{
				m_itemInteractionAreas.Emplace(kvp.Value, Cast<UBoxComponent>(m_interactionArea));
				continue;
			}
			if (interactionArea->ComponentHasTag(name))
				m_itemInteractionAreas.Emplace(kvp.Value, Cast<UBoxComponent>(interactionArea));
		}
	}

	// ------------------------------------

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnCharacterHasItemResponse.AddDynamic(this, &APedestal::OnCharacterHasItemResponse);
	eventHandler->OnPedestalUpdateRequest.AddDynamic(this, &APedestal::OnUpdateRequest);

	m_requestedItemName = "";
}

void APedestal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APedestal::OnUpdateRequest(TArray<int> equippedItems)
{
	m_insertedItemIDs = equippedItems;
	ApplyVisibility();
}

void APedestal::ApplyVisibility()
{
	// Set pedestal animation to end if base key has been inserted
	if (m_insertedItemIDs.Num() > 0)
		SetPedestalAnimationEntry(true);

	// "reset" pedestal
	for (auto kvp : m_itemMeshes)
		kvp.Value->SetVisibility(false);

	// set individual visibility
	for (int i : m_insertedItemIDs)
	{
		if (m_itemMeshes.Contains(i))
			m_itemMeshes[i]->SetVisibility(true);
	}

	// activate individual interactionArea
	for (int i : m_insertedItemIDs)
	{
		if (m_itemInteractionAreas.Contains(i))
			m_itemInteractionAreas[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APedestal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		m_currentElementSlotName = "KeyBase";
		UE_LOG(LogTemp, Warning, TEXT("Set current"));
	}
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, Hit);
}

void APedestal::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		m_currentElementSlotName = "";
	}
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}