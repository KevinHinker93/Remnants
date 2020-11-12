// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AItem.h"
#include "AKeyItem.generated.h"

UCLASS()
class REMNANTS_API UAKeyItem : public UAItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int m_unlockedCraftingRecipeId = -1;
};
