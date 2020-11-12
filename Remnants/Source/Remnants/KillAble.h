// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KillAble.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKillAble : public UInterface
{
	GENERATED_BODY()

	UKillAble(const class FObjectInitializer& ObjectInitializer);
};

class REMNANTS_API IKillAble
{
	GENERATED_BODY()

public:
	virtual void Kill();
};
