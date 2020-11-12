// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootCrate.h"
#include "LootingCrystal.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTS_API ALootingCrystal : public ALootCrate
{
	GENERATED_BODY()
	
public:
	ALootingCrystal();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DestroyCrystal"))
		void DestroyCrystal();

protected:
	virtual void Kill() override;

private:
	FCriticalSection m_criticalSectionHandle;
	bool m_hasBeenLooted;
};
