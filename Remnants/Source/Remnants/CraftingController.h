// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingInternalLogic.h"
#include "CraftingController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class REMNANTS_API UCraftingController : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Craft")
		static int CraftCrystalFromCrystals(int primaryElementId, int secondaryElementId);

private:
	static TMap<CrystalType, AbstractCrystalElement*> Elements;
	static void GenerateElements();
};
