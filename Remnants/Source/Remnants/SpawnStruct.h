// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnStruct.generated.h"

USTRUCT(BlueprintType)
struct FSpawnStruct
{
	GENERATED_USTRUCT_BODY()
		FSpawnStruct() {}

	float m_probability;
	float thresholdMin;
	float thresholdMax;
};