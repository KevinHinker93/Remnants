// Fill out your copyright notice in the Description page of Project Settings.

#include "EventFacade.h"
#include "EventSystemHandlerSingleton.h"

UEventFacade::UEventFacade()
{

}

UEventFacade::~UEventFacade()
{
}

//void UEventFacade::BeginPlay()
//{
//	bool checkSingleton = false;
//	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
//	SetEventHandler(eventHandler);
//}

void UEventFacade::SetEventHandler(UEventSystemHandlerSingleton* eventHandler)
{
	m_eventHandler = eventHandler;

	m_eventHandler->OnItemResponse.AddDynamic(this, &UEventFacade::OnItemResponse);
	m_eventHandler->OnItemInfoResponse.AddDynamic(this, &UEventFacade::OnItemInfoResponse);
	m_eventHandler->OnCraftingOptionResponse.AddDynamic(this, &UEventFacade::OnCraftingOptionResponse);
	m_eventHandler->OnCraftingResponse.AddDynamic(this, &UEventFacade::OnCraftingResponse);
	m_eventHandler->OnPlayerCanCraftResponse.AddDynamic(this, &UEventFacade::OnPlayerCanCraftResponse);
}

const FItemStruct UEventFacade::GetItemInfo(int itemId)
{
	m_state = State::RequestedItemInfo;
	m_tmpItemStruct = FItemStruct(); // "empty"
	m_eventHandler->OnItemInfoRequest.Broadcast(this, itemId);
	return m_tmpItemStruct;
}

UAItem* UEventFacade::CreateItem(int itemId)
{
	m_state = State::RequestedItem;
	m_tmpItemObject = nullptr;
	m_eventHandler->OnItemRequest.Broadcast(this, itemId);
	return m_tmpItemObject;
}

void UEventFacade::SpawnPickup(UAItem* item, const FTransform& transform, ULevel* level)
{
	m_state = State::RequestedPickup;
	m_eventHandler->OnPickupSpawnRequest.Broadcast(this, item, transform, level);
}

void UEventFacade::SpawnPickupById(int itemId, const FTransform& transform, ULevel* level)
{
	m_state = State::RequestedPickup;
	m_eventHandler->OnPickupSpawnByIdRequest.Broadcast(this, itemId, transform, level);
}

void UEventFacade::OnItemResponse(UObject* sender, UAItem* item)
{
	if (sender != this) return;
	m_tmpItemObject = item;
}

void UEventFacade::OnItemInfoResponse(UObject* sender, const FItemStruct& itemInfo)
{
	if (sender != this) return;
	m_tmpItemStruct = itemInfo;
}

UAItem* UEventFacade::CraftItem(UItemCollection* availableItems, int itemId)
{
	m_state = State::RequestedCraft;
	m_tmpItemObject = nullptr;
	m_eventHandler->OnCraftingRequest.Broadcast(this, itemId, availableItems);
	return m_tmpItemObject;
}

int UEventFacade::GetCraftingOption(UItemCollection* availableItems)
{
	m_state = State::RequestedCraftingOption;
	m_tmpInt = -1;
	m_eventHandler->OnCraftingOptionRequest.Broadcast(this, availableItems);
	return m_tmpInt;
}

void UEventFacade::OnCraftingOptionResponse(UObject* sender, int itemId)
{
	if (sender != this) return;
	m_tmpInt = itemId;
}

void UEventFacade::OnCraftingResponse(UObject* sender, UAItem* item)
{
	if (sender != this) return;
	m_tmpItemObject = item;
}

bool UEventFacade::PlayerCanCraft(int itemId)
{
	m_state = State::RequestedCraftingPossible;
	m_tmpBool = false;
	m_eventHandler->OnPlayerCanCraftRequest.Broadcast(this, itemId);
	return m_tmpBool;
}

void UEventFacade::OnPlayerCanCraftResponse(UObject* sender, int itemId, bool canCraft)
{
	if (sender != this) return;
	m_tmpBool = canCraft;
}