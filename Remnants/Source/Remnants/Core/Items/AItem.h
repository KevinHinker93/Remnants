// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "ItemManager.h"
#include "ItemStruct.h"
#include "AItem.generated.h"

UCLASS(Blueprintable, ABSTRACT)
class REMNANTS_API UAItem : public UObject
{
	GENERATED_BODY()
	
public:	
	UAItem();
	UFUNCTION(BlueprintPure)
		int GetId();
	void SetId(int id);
	AActor* GetOwner();
	void SetOwner(AActor* owner);
	UFUNCTION(BlueprintType)
		int AddAmount(int amount); // returns how many could not be added
	UFUNCTION(BlueprintType)
		int RemoveAmount(int amount); // returns how many could not be removed
	UFUNCTION(BlueprintPure)
		int GetAmount();
	UFUNCTION(BlueprintPure)
		int GetMaxAmount();
	void SetMaxAmount(int size);
protected:
	UPROPERTY(SaveGame, VisibleAnywhere)
		int m_itemId = -1;
	UPROPERTY(SaveGame, VisibleAnywhere)
		bool m_idSet = false;
	AActor* m_owner = nullptr;

	UPROPERTY(SaveGame, VisibleAnywhere)
		int m_currentAmount;
	UPROPERTY(SaveGame, VisibleAnywhere)
		int m_maxAmount = 1;
};