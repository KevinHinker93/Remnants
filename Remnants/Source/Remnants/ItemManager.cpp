// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "EventSystemHandlerSingleton.h"
#include "Core/Items/AItem.h"
#include "Core/Items/PickUp.h"
#include "ItemCollection.h"
#include "Engine/World.h"

AItemManager::AItemManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

int AItemManager::GetItemCount()
{
	return m_items.Num();
}

void AItemManager::BeginPlay()
{
	Super::BeginPlay();
	bool checkSingleton = false;
	m_eventSystem = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	// set IDs in item structs
	for (int i = 0; i < m_items.Num(); i++)
		m_items[i].m_id = i;
	UE_LOG(LogTemp, Warning, TEXT("%d items in ItemManager list"), m_items.Num());

	// add events
	m_eventSystem->OnItemRequest.AddDynamic(this, &AItemManager::RequestItem);
	m_eventSystem->OnItemChunkRequest.AddDynamic(this, &AItemManager::RequestItemChunk);
	m_eventSystem->OnItemInfoRequest.AddDynamic(this, &AItemManager::RequestItemInformation);
	m_eventSystem->OnPickupSpawnRequest.AddDynamic(this, &AItemManager::RequestPickupSpawn);
	m_eventSystem->OnPickupSpawnByIdRequest.AddDynamic(this, &AItemManager::RequestPickupSpawnById);
	m_eventSystem->OnCraftingRequest.AddDynamic(this, &AItemManager::CraftItem);
	m_eventSystem->OnCraftingOptionRequest.AddDynamic(this, &AItemManager::FindCraftingOption);
}

void AItemManager::EndPlay(EEndPlayReason::Type endPlayReason)
{
	m_eventSystem->OnItemRequest.RemoveDynamic(this, &AItemManager::RequestItem);
	m_eventSystem->OnItemChunkRequest.RemoveDynamic(this, &AItemManager::RequestItemChunk);
	m_eventSystem->OnItemInfoRequest.RemoveDynamic(this, &AItemManager::RequestItemInformation);
	m_eventSystem->OnPickupSpawnRequest.RemoveDynamic(this, &AItemManager::RequestPickupSpawn);
	m_eventSystem->OnPickupSpawnByIdRequest.RemoveDynamic(this, &AItemManager::RequestPickupSpawnById);
	m_eventSystem->OnCraftingRequest.RemoveDynamic(this, &AItemManager::CraftItem);
	m_eventSystem->OnCraftingOptionRequest.RemoveDynamic(this, &AItemManager::FindCraftingOption);
}

void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// return a new instance of an item by id
UAItem* AItemManager::GetItemById(int Id)
{
	bool invalidId = Id < 0 || Id >= m_items.Num() || !m_items[Id].m_blueprint;
	if (invalidId)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Id: %d"), Id);
		return nullptr;
	}

	auto item = Cast<UAItem>(m_items[Id].m_blueprint->GetDefaultObject());

	auto itemCopy = DuplicateObject(item, item->GetOuter());
	itemCopy->SetId(Id);
	itemCopy->SetMaxAmount(m_items[Id].m_maxStackSize);
	itemCopy->AddAmount(1);
	return itemCopy;
}

void AItemManager::RequestItem(UObject* sender, int itemId)
{
	FString senderName = sender->GetName();
	UE_LOG(LogTemp, Verbose, TEXT("Actor %s requested item: %d"),*senderName, itemId);

	UAItem* item = GetItemById(itemId);
	FItemStruct itemStruct = m_items[itemId];

	// send item
	m_eventSystem->OnItemResponse.Broadcast(sender, item);
}

void AItemManager::RequestItemChunk(UObject* sender, TArray<int>& itemIds)
{
	FString senderName = sender->GetName();
	UE_LOG(LogTemp, Verbose, TEXT("Actor %s requested items"), *senderName);    

	TArray<UAItem*> items;
	items.SetNum(itemIds.Num());

	for (int i = 0; i < itemIds.Num(); i++)
	{
		UAItem* item;

		item = GetItemById(itemIds[i]);
		items[i] = item;
	}
	m_eventSystem->OnItemChunkResponse.Broadcast(sender, items);
}

void AItemManager::RequestItemInformation(UObject* sender, int itemId)
{
	if (itemId < 0 || itemId >= m_items.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Requested Item Information: no item with ID %d"), itemId);
		return;
	}

	FItemStruct itemInfo = m_items[itemId];

	m_eventSystem->OnItemInfoResponse.Broadcast(sender, itemInfo);
}

void AItemManager::RequestPickupSpawn(UObject* sender, UAItem* item, const FTransform& transform, ULevel* level)
{
	int itemId = item->GetId();
	if (itemId < 0 || itemId >= m_items.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find item information for Id %d"), itemId);
		return;
	}

	FItemStruct itemStruct = m_items[itemId];

	if (m_pickupBp)
	{
		// spawn pickup & add item
		FActorSpawnParameters params{};
		params.OverrideLevel = level;

		auto pickup = GetWorld()->SpawnActor<APickUp>(m_pickupBp, params);
		auto meshComp = Cast<UStaticMeshComponent>(pickup->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		meshComp->SetStaticMesh(itemStruct.m_groundMesh);
		pickup->SetItem(item);
		item->SetOwner(pickup);

		// set position of pickup to sender position
		FTransform pickupTransform = pickup->GetActorTransform();
		pickupTransform.CopyTranslation(transform);
		pickup->SetActorTransform(pickupTransform);

		UE_LOG(LogTemp, Display, TEXT("Spawned PickUp with Id %d"), item->GetId());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pickup BP Class not found"));
		return;
	}
}

void AItemManager::RequestPickupSpawnById(UObject* sender, int id, const FTransform& transform, ULevel* level)
{
	UAItem* item = GetItemById(id);
	RequestPickupSpawn(sender, item, transform, level);
}

void AItemManager::CraftItem(UObject* sender, int itemId, UItemCollection* availableItems)
{
	FItemStruct itemInfo = m_items[itemId];

	for (auto requirement : itemInfo.m_craftingRequirements)
		availableItems->RemoveItem(requirement.m_itemId, requirement.m_amount);

	m_eventSystem->OnItemResponse.Broadcast(sender, GetItemById(itemId));
	m_eventSystem->OnCraftedItem.Broadcast();
	UE_LOG(LogTemp, Display, TEXT("Successfully crafted item with ID %d"), itemId);
}

void AItemManager::FindCraftingOption(UObject* sender, UItemCollection* availableItems)
{
	UE_LOG(LogTemp, Display, TEXT("Available Crafting Items:"));
	availableItems->Print();

	// loop through all potential crafting options
	for (int itemId = 0; itemId < m_items.Num(); itemId++)
	{
		FItemStruct itemInfo = m_items[itemId];

		if (!itemInfo.m_craftable)
			continue;

		bool adequateResources = true;

		// iterate through requirements
		for (auto requirement : itemInfo.m_craftingRequirements)
		{
			// cancel if requirement is not met
			if (availableItems->GetItemAmount(requirement.m_itemId) < requirement.m_amount)
			{
				adequateResources = false;
				break;
			}
		}

		if (adequateResources)
		{
			m_eventSystem->OnCraftingOptionResponse.Broadcast(sender, itemId);
			UE_LOG(LogTemp, Display, TEXT("Found crafting option with ID %d"), itemId);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Could not find any crafting options"));
}