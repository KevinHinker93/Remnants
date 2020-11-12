// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "BlueprintProbabilityStruct.generated.h"

USTRUCT(BlueprintType)
struct FBlueprintProbabilityStruct
{
	GENERATED_USTRUCT_BODY()
		FBlueprintProbabilityStruct();

	UPROPERTY(EditAnywhere)
		float m_probability;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> m_blueprint;
};