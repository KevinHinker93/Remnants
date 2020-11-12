// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Items/AItem.h"
#include "ItemCollection.h"
#include "EventFacade.generated.h"

UCLASS()
class REMNANTS_API UEventFacade : public UObject
{
	GENERATED_BODY()

public:
	UEventFacade();
	~UEventFacade();

	enum State
	{
		None,
		RequestedCraft,
		RequestedItem,
		RequestedItemInfo,
		RequestedPickup,
		RequestedCraftingOption,
		RequestedCraftingPossible,
		RequestedPlayerItem
	};

	//virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "EventFacade")
	const FItemStruct GetItemInfo(int itemId);
	UFUNCTION(BlueprintCallable, Category = "EventFacade")
	UAItem* CreateItem(int itemId);
	void SpawnPickup(UAItem* item, const FTransform& transform, ULevel* level);
	void SpawnPickupById(int itemId, const FTransform& transform, ULevel* level);
	UFUNCTION(BlueprintCallable, Category = "EventFacade")
	UAItem* CraftItem(UItemCollection* availableItems, int itemId);
	UFUNCTION(BlueprintCallable, Category = "EventFacade")
	int GetCraftingOption(UItemCollection* availableItems);
	UFUNCTION(BlueprintCallable, Category = "EventFacade")
	bool PlayerCanCraft(int itemId);

	void SetEventHandler(class UEventSystemHandlerSingleton* eventHandler);
protected:
	UFUNCTION()
		void OnItemResponse(UObject* sender, UAItem* item);
	UFUNCTION()
		void OnItemInfoResponse(UObject* sender, const FItemStruct& itemInfo);
	UFUNCTION()
		void OnCraftingResponse(UObject* sender, UAItem* item);
	UFUNCTION()
		void OnCraftingOptionResponse(UObject* sender, int itemId);
	UFUNCTION()
		void OnPlayerCanCraftResponse(UObject* sender, int itemId, bool canCraft);
private:
	UAItem* m_tmpItemObject;
	FItemStruct m_tmpItemStruct;
	int m_tmpInt;
	bool m_tmpBool;

	UEventSystemHandlerSingleton* m_eventHandler;
	State m_state;
};