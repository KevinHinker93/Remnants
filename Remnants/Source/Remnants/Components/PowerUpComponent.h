// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerUpComponent.generated.h"

class UBuff;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UPowerUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPowerUpComponent();
	// Add one powerUp to the current pickup counter
	UFUNCTION(BlueprintCallable, Category = "Power Up")
		void AddPowerUpPickup();

	int GetCurrentPowerUpCount()
	{
		return m_currentPowerUpPickUps;
	}

	void SetCurrentPowerUpCount(int powerUpCount)
	{
		m_currentPowerUpPickUps = powerUpCount;
		OnPowerUpCountChanged(m_currentPowerUpPickUps);

		ActivateBuff(false);
	}

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPowerUpMaxCountChanged"))
		void OnPowerUpMaxCountChanged(int idx, int maxPowerUpCount);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPowerUpCountChanged"))
		void OnPowerUpCountChanged(int powerUpCount);
	// Activates buff and displays gui notification if enabled
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPowerUpBuffActivated"))
		void OnPowerUpBuffActivated(FName buffName, bool notification);

	// How many buffs can be activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PowerUp)
		int m_buffCount;
	// Stores the buffs that the owner will receive when he has reached the specified count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PowerUp)
		TMap<int, TSubclassOf<UBuff>> m_buffsAtPickUpCount;

	// How many pickups the entity currently has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Powerup")
		int m_currentPowerUpPickUps;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:	
	bool m_firedNeededPickUpCountEvent;
	int m_maxPowerUpPickUps;
	FCriticalSection m_criticalSection;
		
	// Activates buff and displays gui notification if enabled
	void ActivateBuff(bool notification);
};
