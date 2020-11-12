// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractible.h"
#include "CraftingStation.generated.h"

class ACharacter;

UCLASS()
class REMNANTS_API ACraftingStation : public AAInteractible
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACraftingStation();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OpenInventory"))
		void OpenInventory(const  ACharacter* character);

	// AInteractible functions
	virtual void InteractionLogic(class ACharacter* const character) override;
	virtual void InitializeInteractible() override;
	virtual void DeInitialize() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
};
