// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ItemStruct.generated.h"

class UAItem;

USTRUCT(BlueprintType)
struct FCraftingRequirementStruct
{
	GENERATED_USTRUCT_BODY()
		FCraftingRequirementStruct() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_itemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_amount;
};

USTRUCT(BlueprintType)
struct FCraftingOptionRequirementStruct
{
	GENERATED_USTRUCT_BODY()
		FCraftingOptionRequirementStruct() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_itemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_targetAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_availableAmount;
	
	bool hasSufficientCount()
	{
		return m_availableAmount >= m_targetAmount;
	}
};

USTRUCT(BlueprintType)
struct FCraftingOptionStruct
{
	GENERATED_USTRUCT_BODY()
		FCraftingOptionStruct() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_itemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCraftingOptionRequirementStruct> m_craftingRequirements;

	bool hasSufficientResources()
	{
		for (auto requirement : m_craftingRequirements)
			if (!requirement.hasSufficientCount())
				return false;
		
		return true;
	}
};

USTRUCT(BlueprintType)
struct FItemStruct
{
	GENERATED_USTRUCT_BODY()
		FItemStruct() {}

	int m_id = -1;

	int GetId();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UAItem> m_blueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText m_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> m_tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
		FText m_description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* m_displayImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor m_displayColor = FColorList::Grey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* m_groundMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_stackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_maxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_craftable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCraftingRequirementStruct> m_craftingRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_canBeDisassembled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCraftingRequirementStruct> m_disassembledItems;
};