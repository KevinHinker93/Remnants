// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "EventSystemHandlerSingleton.h"
#include "Kismet/KismetMathLibrary.h"
#include "Core/Items/AKeyItem.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	//m_maxInventorySize = 10;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();	

	m_items = NewObject<UItemCollection>();
	m_items->SetMaxSlotCount(m_maxInventorySize);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPickup.AddDynamic(this, &UInventoryComponent::PickupItem);
	eventHandler->OnCharacterHasItemRequest.AddDynamic(this, &UInventoryComponent::OnCharacterHasItemRequest);
	eventHandler->OnCharacterHasItemsRequest.AddDynamic(this, &UInventoryComponent::OnCharacterHasItemsRequest);
	eventHandler->OnPlayerCanCraftRequest.AddDynamic(this, &UInventoryComponent::OnCanCraftItemRequest);
	eventHandler->OnKeyHolderDied.AddDynamic(this, &UInventoryComponent::OnKeyHolderDied);
	eventHandler->OnBossDropItems.AddDynamic(this, &UInventoryComponent::OnBossDied);
}

void UInventoryComponent::EndPlay(EEndPlayReason::Type endPlayReason)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPickup.RemoveDynamic(this, &UInventoryComponent::PickupItem);
	eventHandler->OnCharacterHasItemRequest.RemoveDynamic(this, &UInventoryComponent::OnCharacterHasItemRequest);
	eventHandler->OnCharacterHasItemsRequest.RemoveDynamic(this, &UInventoryComponent::OnCharacterHasItemsRequest);
	eventHandler->OnPlayerCanCraftRequest.RemoveDynamic(this, &UInventoryComponent::OnCanCraftItemRequest);
	eventHandler->OnKeyHolderDied.RemoveDynamic(this, &UInventoryComponent::OnKeyHolderDied);
	eventHandler->OnBossDropItems.RemoveDynamic(this, &UInventoryComponent::OnBossDied);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UItemCollection* UInventoryComponent::GetItemCollection()
{
	return m_items;
}

void UInventoryComponent::PickupItem(UObject* pickUpReceiver, UAItem* item)
{
	//UE_LOG(LogTemp, Warning, TEXT("Received pickup notification"));
	if (pickUpReceiver == GetOwner())
	{
		if (item)
		{
			// add item to inventory
			UE_LOG(LogTemp, Display, TEXT("Picked up item with id %d"), item->GetId());
			m_items->AddItem(item);
			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
			auto itemInfo = eventHandler->GetEventFacade()->GetItemInfo(item->GetId());

			int stackSize = item->GetAmount();
			eventHandler->OnItemReceivedNotification.Broadcast(itemInfo.m_displayImage, itemInfo.m_name.ToString(), stackSize);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Picked up item does not exist"));
		}

		m_items->Print();
	}
}

void UInventoryComponent::OnKeyHolderDied(int keyItemId)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	auto item = eventHandler->GetEventFacade()->CreateItem(keyItemId);
	auto itemInfo = eventHandler->GetEventFacade()->GetItemInfo(keyItemId);
	GetItemCollection()->AddItem(item);
	int stackSize = item->GetAmount();

	// Tell UI to display the received item
	eventHandler->OnItemReceivedNotification.Broadcast(itemInfo.m_displayImage, itemInfo.m_name.ToString(), stackSize);
	// Progress achievement event
	eventHandler->OnReceiveElementalKey.Broadcast(itemInfo.m_name.ToString());

	auto keyItem = Cast<UAKeyItem>(item);
	AddCraftingRecipe(keyItem);
	auto keyItemInfo = eventHandler->GetEventFacade()->GetItemInfo(keyItem->m_unlockedCraftingRecipeId);
	FString msg = "Crafting unlocked: ";
	eventHandler->OnDisplayUIMessage.Broadcast(msg + keyItemInfo.m_name.ToString(), 4.75f);

	// show tutorial
	TryShowCombineTutorial();
}

void UInventoryComponent::OnBossDied(const TArray<int>& droppedItemIds, const int& amount)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	for (auto itemIdIt : droppedItemIds)
	{
		auto item = eventHandler->GetEventFacade()->CreateItem(itemIdIt);
		auto itemInfo = eventHandler->GetEventFacade()->GetItemInfo(itemIdIt);
		GetItemCollection()->AddItem(item, amount);

		// Tell UI to display the received item
		eventHandler->OnItemReceivedNotification.Broadcast(itemInfo.m_displayImage, itemInfo.m_name.ToString(), amount);
	}
}

void UInventoryComponent::OnCharacterHasItemRequest(AActor* sender, AActor* receivingCharacter, int itemId)
{
	if (GetOwner() == receivingCharacter)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		int itemCount = m_items->GetItemAmount(itemId);
		if (itemCount > 0)
		{
			m_items->RemoveItem(itemId, 1);
			eventHandler->OnCharacterHasItemResponse.Broadcast(sender, true);
		}
		else
			eventHandler->OnCharacterHasItemResponse.Broadcast(sender, false);
	}
}

void UInventoryComponent::OnCharacterHasItemsRequest(AActor* sender, AActor* receivingCharacter, TArray<int> itemIds)
{
	if (GetOwner() == receivingCharacter)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		TArray<int> availableItemIds;

		for (auto itemId : itemIds)
		{
			int itemCount = m_items->GetItemAmount(itemId);
			if (itemCount > 0)
			{
				availableItemIds.Add(itemId);
				m_items->RemoveItem(itemId, 1);
			}
		}
		eventHandler->OnCharacterHasItemsResponse.Broadcast(sender, availableItemIds);
	}
}

float UInventoryComponent::GetInventorySizeFloat() const
{
	return m_items->GetSlotCount();
}

UAItem* UInventoryComponent::GetItem(int itemIdx)
{
	return m_items->GetStack(itemIdx);
}

int UInventoryComponent::GetInventorySize()
{
	return m_items->GetSlotCount();
}

void UInventoryComponent::OnCanCraftItemRequest(UObject* sender, int itemId)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPlayerCanCraftResponse.Broadcast(sender, itemId, m_craftableItemIds.Contains(itemId));
}

void UInventoryComponent::TryShowCombineTutorial()
{
	if (m_gemCombineTutorialCompleted || m_craftableItemIds.Num() != 1) return;

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	auto info = eventHandler->GetEventFacade()->GetItemInfo(m_craftableItemIds[0]);
		
	if (info.m_craftable)
	{
		eventHandler->OnCombineTutorialEnabled.Broadcast(0);
		m_gemCombineTutorialCompleted = true;
	}
}

void UInventoryComponent::AddCraftingRecipe(UAKeyItem* keyItem)
{
	if (keyItem && keyItem->m_unlockedCraftingRecipeId >= 0 && !m_craftableItemIds.Contains(keyItem->m_unlockedCraftingRecipeId))
		m_craftableItemIds.Add(keyItem->m_unlockedCraftingRecipeId);
}