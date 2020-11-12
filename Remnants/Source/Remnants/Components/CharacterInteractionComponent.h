// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interactible.h"
#include "CharacterInteractionComponent.generated.h"

class AAInteractible;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UCharacterInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterInteractionComponent();

	void Interact();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type type) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:	
	UFUNCTION()
		void InitializeInteractible(AAInteractible* interactible);
	UFUNCTION()
		void DeInitializeInteractible(AAInteractible* interactible);
	UPROPERTY()
		AAInteractible* m_interactibleObject;
		
	
};
