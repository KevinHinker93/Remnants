// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h" 
#include "RemnantsGameInstance.generated.h"

UCLASS()
class REMNANTS_API URemnantsGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	URemnantsGameInstance(const FObjectInitializer& ObjectInitializer);
	//~RemnantsGameInstance();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName m_currentLevelName = "PrototypeLVL";

};
