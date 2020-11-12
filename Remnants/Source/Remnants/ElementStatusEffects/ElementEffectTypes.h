// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ElementEffectType : uint8
{
	Fire 	UMETA(DisplayName = "Fire"),
	Earth 	UMETA(DisplayName = "Earth"),
	Water 	UMETA(DisplayName = "Water"),
	Wind 	UMETA(DisplayName = "Wind"),

	Lava 	UMETA(DisplayName = "Lava"),
	Thunder 	UMETA(DisplayName = "Thunder"),
	Steam 	UMETA(DisplayName = "Steam"),
	Ice 	UMETA(DisplayName = "Ice"),
	Acid 	UMETA(DisplayName = "Acid"),
	Crystal 	UMETA(DisplayName = "Crystal"),

	Light 	UMETA(DisplayName = "Light"),
	Dark 	UMETA(DisplayName = "Dark"),
	Plasma 	UMETA(DisplayName = "Plasma")
};
