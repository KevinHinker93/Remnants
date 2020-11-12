// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AItem.h"
#include "../../CraftingInternalLogic.h"
#include "AElementShard.generated.h"

UCLASS()
class REMNANTS_API UAElementShard : public UAItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TEnumAsByte<CrystalType> m_shardType = TEnumAsByte<CrystalType>(CrystalType::None);
	
	
};
