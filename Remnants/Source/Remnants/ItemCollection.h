// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Items/AItem.h"
#include "Object.h"
#include "ItemCollection.generated.h"

UCLASS(Blueprinttype, meta = (BlueprintSpawnableComponent))
class REMNANTS_API UItemCollection : public UObject
{
	GENERATED_BODY()

public:
	void AddAllowedItemId(int itemId);

	UItemCollection(const FObjectInitializer & ObjectInitializer);

	void SetMaxSlotCount(int count);
	int GetMaxSlotCount();
	int GetSlotCount();

	// return value: how many instances could not be added
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		int AddItem(int itemId, int amount = 1, bool mergeStacks = true, UItemCollection* source = nullptr);
	int AddItem(UAItem* item, bool mergeStacks = true, UItemCollection* source = nullptr);
	int AddItem(UAItem* item, int amount, bool mergeStacks = true, UItemCollection* source = nullptr);
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		int RemoveItem(int itemId, int amount);
	UFUNCTION(BlueprintPure, Category = "ItemCollection")
		int GetItemAmount(int itemId);
	int GetTotalItemAmount();

	bool RemoveStack(UAItem* stack);
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		bool RemoveStack(int idx);	
	UFUNCTION(BlueprintPure, Category = "ItemCollection")
		UAItem* GetStack(int idx);	
	UFUNCTION(BlueprintPure, Category = "ItemCollection")
		const TArray<UAItem*>& GetItems();

	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		void Merge(UItemCollection* other, bool keepSize = true);
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		void CopyTo(UItemCollection* other, bool deepCopy);
	UItemCollection* Clone(bool deepClone);
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		void Clear();
	void Print();
	UFUNCTION(BlueprintCallable, Category = "ItemCollection")
		bool HasEitherItem(TArray<int> itemIds);
protected:
	int m_maxSlotCount = 30;
	UPROPERTY()
		TArray<UAItem*> m_items;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> m_allowedItemIds;
	bool ItemIsAllowed(int itemId);

	int TryFillStacks(UAItem* item, int amount, UItemCollection* source = nullptr);
	int AddStacks(UAItem* stack, int amount, UItemCollection* source = nullptr);
	bool AddStack(UAItem* stack, UItemCollection* source = nullptr);
};
