// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDamageEffectiveness : uint8
{
	Normal 	UMETA(DisplayName = "Normal"),
	Effective 	UMETA(DisplayName = "Effective"),
	UnEffective 	UMETA(DisplayName = "UnEffective")
};