// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageEffectiveness.h"
#include "ItemEffectivenessData.generated.h"

USTRUCT(BlueprintType)
struct FItemEffectivenessData
{
	GENERATED_USTRUCT_BODY()
public:
	FItemEffectivenessData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int m_itemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDamageEffectiveness m_effectiveness;
};