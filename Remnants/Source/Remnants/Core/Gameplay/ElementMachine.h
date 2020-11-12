// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractible.h"
#include "ElementMachine.generated.h"

UCLASS()
class REMNANTS_API AElementMachine : public AAInteractible
{
	GENERATED_BODY()
	
public:	
	AElementMachine();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SwitchBarrier"))
		void SwitchBarrier();

	// AInteractible functions
	virtual void InteractionLogic(class ACharacter* const character) override;
	virtual void InitializeInteractible() override;
	virtual void DeInitialize() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementMachine)
		int m_elementCloudIdentifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementMachine)
		float m_interactionInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Texts")
		FName m_interactionText;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:	
	FTimerHandle m_interactionTimerHandle;
	bool m_canInteract;

	UFUNCTION()
		void EnableInteraction();
};
