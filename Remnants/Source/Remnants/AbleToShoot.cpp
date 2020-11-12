// Fill out your copyright notice in the Description page of Project Settings.

#include "AbleToShoot.h"

// Add default functionality here for any IAbleToShoot functions that are not pure virtual.

UAbleToShoot::UAbleToShoot(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IAbleToShoot::WeaponFireAnimation()
{
}

USceneComponent* IAbleToShoot::GetMuzzelLoc()
{
	return nullptr;
}

float IAbleToShoot::GetCurrentOverheatPercentage() const
{
	return 0.0f;
}

float IAbleToShoot::GetDamageMultiplier() const
{
	return 1.0f;
}
