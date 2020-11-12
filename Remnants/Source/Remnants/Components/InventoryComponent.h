// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Items/AItem.h"
#include "ItemCollection.h"
#include "EventSystemHandlerSingleton.h"
#include "InventoryComponent.generated.h"

class UAItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UFUNCTION(BlueprintPure, Category = "Inventory")
		UItemCollection* GetItemCollection();
	UFUNCTION(BlueprintPure, Category = "Inventory")
		int GetInventorySize();
	UFUNCTION(BlueprintPure, Category = "Inventory")
		UAItem* GetItem(int itemIdx);
	UFUNCTION(BlueprintPure, Category = "Inventory")
		float GetInventorySizeFloat() const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void TryShowCombineTutorial();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetGemCombineTutorialCompleted( bool completed ){ m_gemCombineTutorialCompleted = completed; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void AddCraftingRecipe(class UAKeyItem* keyItem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<int> m_craftableItemIds;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		float m_maxInventorySize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		bool m_gemCombineTutorialCompleted;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Event listener
	UFUNCTION()
		void PickupItem(UObject* pickUpReceiver, UAItem* item);

	UFUNCTION()
		void OnCharacterHasItemRequest(AActor* sender, AActor* receivingCharacter, int itemId);

	UFUNCTION()
		void OnCharacterHasItemsRequest(AActor* sender, AActor* receivingCharacter, TArray<int> itemIds);
	
	UFUNCTION()
		void OnCanCraftItemRequest(UObject* sender, int itemId);

	UFUNCTION()
		void OnKeyHolderDied(int keyItemId);
	UFUNCTION()
		void OnBossDied(const TArray<int>& droppedItemIds, const int& amount);
private:
	UPROPERTY()
		UItemCollection* m_items;
};
