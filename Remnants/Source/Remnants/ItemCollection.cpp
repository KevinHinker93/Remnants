// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemCollection.h"
#include "EventSystemHandlerSingleton.h"

UItemCollection::UItemCollection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void UItemCollection::SetMaxSlotCount(int count)
{
	if (count < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot set max inventory size to negative value"));
		return;
	}

	m_maxSlotCount = count;
	if (count < m_items.Num()) // items would need to be discarded
	{
		int diff = m_items.Num() - count;
		UE_LOG(LogTemp, Warning, TEXT("Inventory size reduced: Discarded %d items"), diff);
		for (int i = 0; i < diff; i++)
			m_items.RemoveAt(m_items.Num() - i);
	}
}

int UItemCollection::GetMaxSlotCount()
{
	return m_maxSlotCount;
}

const TArray<UAItem*>& UItemCollection::GetItems()
{
	return m_items;
}

int UItemCollection::AddItem(int itemId, int amount, bool mergeStacks, UItemCollection* source)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	UAItem* item = eventHandler->GetEventFacade()->CreateItem(itemId);
	return AddItem(item, amount, mergeStacks, source);
}

int UItemCollection::AddItem(UAItem* item, bool mergeStacks, UItemCollection* source)
{
	return AddItem(item, item->GetAmount(), mergeStacks, source);
}

int UItemCollection::AddItem(UAItem* item, int amount, bool mergeStacks, UItemCollection* source)
{
	if (item == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not add item: Item is null"));
		return amount;
	}

	if (!ItemIsAllowed(item->GetId()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not add item: Item Id %d not allowed in this collection"), item->GetId());
		return amount;
	}

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	auto itemInfo = eventHandler->GetEventFacade()->GetItemInfo(item->GetId());

	if (itemInfo.m_stackable)
	{
		// try to fill existing stacks
		if (mergeStacks)
			amount = TryFillStacks(item, amount);

		// add stacks for remaining objects
		amount = AddStacks(item, amount, source);

		return amount;
	}
	else // add separate object for each stack
	{
		// add original
		if (AddStack(item, source))
		{
			amount--;

			// add clones for rest of amount
			for (int i = 1; i < amount; i++)
			{
				// TODO: Check if duplicating works...
				auto duplicateItem = NewObject<UAItem>(this, item->GetClass(), item->GetFName(), item->GetFlags(), item);
				duplicateItem->SetId(item->GetId());
				bool success = AddStack(duplicateItem, source);
				if (!success)
					break;
				else
					amount--;
			}
		}

		return amount;
	}
}

bool UItemCollection::AddStack(UAItem* item, UItemCollection* source)
{
	if (m_items.Num() < m_maxSlotCount) // free slot in inventory
	{
		m_items.Add(item);

		if (source)
			source->RemoveItem(item->GetId(), item->GetAmount());

		return true;
	}
	else // inventory is full
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not add item: Inventory full"));
		return false;
	}
}

bool UItemCollection::RemoveStack(UAItem* stack)
{
	if (!m_items.Contains(stack))
		return false;

	m_items.RemoveSingle(stack);
	return true;
}

int UItemCollection::AddStacks(UAItem* item, int amount, UItemCollection* source)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	while (amount > 0)
	{
		auto stack = eventHandler->GetEventFacade()->CreateItem(item->GetId());
		amount = stack->AddAmount(amount - 1); // stack spawns with amount of 1

		bool success = AddStack(stack, source);
		if (success)
		{
			UE_LOG(LogTemp, Display, TEXT("Added new stack with id %d, %d / %d"), item->GetId(), stack->GetAmount(), stack->GetMaxAmount());
		}
		else
			return amount;
	}
	return 0;
}

bool UItemCollection::ItemIsAllowed(int itemId)
{
	return m_allowedItemIds.Num() == 0 || m_allowedItemIds.Contains(itemId);
}

bool UItemCollection::RemoveStack(int idx)
{
	if (idx >= 0 && idx < m_items.Num())
	{
		m_items.RemoveAt(idx);
		return true;
	}
	
	return false;
}

int UItemCollection::RemoveItem(int itemId, int amount)
{
	// iterate through all stacks
	for (auto stack : m_items)
	{
		if (stack->GetId() == itemId)
		{
			amount = stack->RemoveAmount(amount);

			// remove stack if empty
			if (stack->GetAmount() == 0)
				RemoveStack(stack);

			if (amount == 0)
				return 0;
		}
	}

	return amount;
}

int UItemCollection::GetSlotCount()
{
	return m_items.Num();
}

UAItem* UItemCollection::GetStack(int idx)
{
	if (idx >= 0 && idx < m_items.Num())
		return m_items[idx];

	return nullptr;
}

void UItemCollection::CopyTo(UItemCollection* other, bool deepCopy)
{
	if (this == other)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to copy item collection to itself. Aborting"));
		return;
	}
	other->Clear();
	other->SetMaxSlotCount(GetMaxSlotCount());
	for (auto item : m_items)
	{
		if (deepCopy)
			other->AddStack(NewObject<UAItem>(other, item->GetClass(), item->GetFName(), item->GetFlags(), item));
		else
			other->AddStack(item);
	}
}

UItemCollection* UItemCollection::Clone(bool deepClone)
{
	UItemCollection* other = NewObject<UItemCollection>();
	CopyTo(other, deepClone);
	return other;
}

void UItemCollection::Clear()
{
	m_items.Empty();
}

int UItemCollection::GetItemAmount(int itemId)
{
	int amount = 0;

	for (auto stack : m_items)
		if (stack->GetId() == itemId)
			amount += stack->GetAmount();

	return amount;
}

void UItemCollection::AddAllowedItemId(int itemId)
{
	m_allowedItemIds.Add(itemId);
}

int UItemCollection::GetTotalItemAmount()
{
	int amount = 0;

	for (auto item : m_items)
		amount += item->GetAmount();

	return amount;
}

void UItemCollection::Merge(UItemCollection* other, bool keepSize)
{
	if (this == other)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to merge item collection into itself. Aborting"));
		return;
	}
	for (int i = 0; i < other->GetSlotCount(); i++)
	{
		auto item = other->GetStack(i);
		//int remainder = other->GetTotalItemAmount();
		int remainder = AddItem(item);

		if (!keepSize)
		{
			while (remainder > 0)
			{
				m_maxSlotCount++;
				remainder = AddItem(item);
			}
		}
	}
}

int UItemCollection::TryFillStacks(UAItem* item, int amount, UItemCollection* source)
{
	for (auto targetStack : m_items)
	{
		if (!targetStack)
		{
			UE_LOG(LogTemp, Error, TEXT("Target stack was null"));
			continue;
		}

		if (item->GetId() == targetStack->GetId())
		{
			int previousAmount = targetStack->GetAmount();
			amount = targetStack->AddAmount(amount);
			UE_LOG(LogTemp, VeryVerbose, TEXT("Added %d instances to stack with item id %d, %d / %d"), targetStack->GetAmount() - previousAmount, item->GetId(), targetStack->GetAmount(), targetStack->GetMaxAmount());

			if (amount == 0) 
				return 0;
		}
	}

	return amount;
}

void UItemCollection::Print()
{
	UE_LOG(LogTemp, Display, TEXT("------------- ITEM COLLECTION --------------"));
	UE_LOG(LogTemp, Display, TEXT("Stack Count: %d / %d"), GetSlotCount(), GetMaxSlotCount());

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	for (UAItem* item : m_items)
	{
		auto itemInfo = eventHandler->GetEventFacade()->GetItemInfo(item->GetId());
		UE_LOG(LogTemp, Display, TEXT("Item Name: %s, Count: %d / %d"), *itemInfo.m_name.ToString(), item->GetAmount(), item->GetMaxAmount());
	}

	UE_LOG(LogTemp, Display, TEXT("--------------------------------------"));
}

bool UItemCollection::HasEitherItem(TArray<int> itemIds)
{
	for (int id : itemIds)
		for (auto item : m_items)
			if (item->GetId() == id)
				return true;

	return false;
}