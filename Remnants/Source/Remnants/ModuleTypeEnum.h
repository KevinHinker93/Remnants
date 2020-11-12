// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ModuleType : uint8
{
	LongRange 	UMETA(DisplayName = "LongRange"),
	ShortRange 	UMETA(DisplayName = "ShortRange"),
};
