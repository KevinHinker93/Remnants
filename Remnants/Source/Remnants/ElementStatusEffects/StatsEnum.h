// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EntityStats : uint8
{
	FiringSpeed 	UMETA(DisplayName = "FiringSpeed"),
	HealthRegen		UMETA(DisplayName = "HealthRegen"),
	MovementSpeed 	UMETA(DisplayName = "MovementSpeed"),
	DamageResistance 	UMETA(DisplayName = "DamageResistance"),
	MaxShield 	UMETA(DisplayName = "MaxShield"),
	OverheatingFactor 	UMETA(DisplayName = "OverheatingFactor"),
	Damage 	UMETA(DisplayName = "Damage"),
	CurrentShield 	UMETA(DisplayName = "CurrentShield"),
	MaxElementEffectResistance 	UMETA(DisplayName = "MaxElementEffectResistance"),
	ElementEffectImpactFac 	UMETA(DisplayName = "ElementEffectImpactFac"),
	MaxHealth 	UMETA(DisplayName = "MaxHealth"),
	FiringStrat 	UMETA(DisplayName = "FiringStrat"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EntityAction : uint8
{
	Firing 	UMETA(DisplayName = "Firing"),
	Move 	UMETA(DisplayName = "Move"),
	All 	UMETA(DisplayName = "All"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EntityState : uint8
{
	Paralysed 	UMETA(DisplayName = "Paralysed"),
	Frozen 	UMETA(DisplayName = "Frozen"),
};