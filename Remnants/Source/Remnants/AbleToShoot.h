// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbleToShoot.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAbleToShoot : public UInterface
{
	GENERATED_BODY()

	UAbleToShoot(const class FObjectInitializer& ObjectInitializer);
};

class REMNANTS_API IAbleToShoot
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void WeaponFireAnimation();
	UFUNCTION(BlueprintCallable, Category = AbleToShoot)
		virtual class USceneComponent* GetMuzzelLoc();
	virtual float GetCurrentOverheatPercentage() const;
	virtual float GetDamageMultiplier() const;
};
