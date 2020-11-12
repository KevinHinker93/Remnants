// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class StaminaStates : uint8
{
	Deplete 	UMETA(DisplayName = "Deplete"),
	Exhausted 	UMETA(DisplayName = "Exhausted"),
	Fit 	UMETA(DisplayName = "Fit"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();
	bool IsExhausted();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings", meta = (AllowPrivateAccess = "true"))
		float m_maxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings", meta = (AllowPrivateAccess = "true"))
		float m_staminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings", meta = (AllowPrivateAccess = "true"))
		float m_staminaRegen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings", meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "1.0"))
		float m_fitThreshold; // If stamina is under a specific threshold, the entity will be exhausted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina Settings", meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "1.0"))
		float m_exhaustedRegenMult; // If exhausted the stamina will regenerate slower

	StaminaStates m_staminaState;
	float m_currentStamina;
	
	UFUNCTION()
		void OnSprinting(APawn* sprintingPawn);
	UFUNCTION()
		void OnStopSprinting(APawn* sprintingPawn);
	
};
