// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "SlateTextures.h"
#include "Core/Items/PickUp.h"
#include "ItemStruct.h"
#include "ItemManager.generated.h"

class UAItem;
class UItemCollection;
class UEventSystemHandlerSingleton;

UCLASS()
class REMNANTS_API AItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemManager();
	UFUNCTION(BlueprintPure, Category = "Items")
		int GetItemCount();

protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<APickUp> m_pickupBp;
	UPROPERTY(EditAnywhere, Category = "Items")
		TArray<FItemStruct> m_items;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason);
	virtual void Tick(float DeltaTime) override;

private:
	// factory methods
	UFUNCTION()
		void RequestItem(UObject* sender, int itemId);
	UFUNCTION()
		void RequestPickupSpawn(UObject* sender, UAItem* item, const FTransform& transform, ULevel* level);
	UFUNCTION()
		void RequestPickupSpawnById(UObject* sender, int id, const FTransform& transform, ULevel* level);
	UFUNCTION()
		void RequestItemChunk(UObject* sender, TArray<int>& itemIds);
	UFUNCTION()
		void RequestItemInformation(UObject* sender, int itemId);
	UFUNCTION()
		void CraftItem(UObject* sender, int itemId, UItemCollection* availableItems);
	UFUNCTION()
		void FindCraftingOption(UObject* sender, UItemCollection* availableItems);

	UAItem* GetItemById(int Id);

	UPROPERTY()
		UEventSystemHandlerSingleton* m_eventSystem;
};