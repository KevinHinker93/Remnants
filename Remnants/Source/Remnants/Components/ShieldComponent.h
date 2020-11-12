// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShieldComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBreak);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDamage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldComponent();

	// Returns current shield
	UFUNCTION(Blueprintcallable, Category = "Shield")
		float GetShield() const;

	UFUNCTION(Blueprintcallable, Category = "Shield")
		float GetCurrentHealthPercentage() const;

	UPROPERTY(BlueprintAssignable, Category= "ShieldBreak")
		FOnShieldBreak OnShieldBreak;

	UPROPERTY(BlueprintAssignable, Category= "StartShield")
		FOnActivateShield OnActivateShield;

	UPROPERTY(BlueprintAssignable, Category= "ShieldDamage")
		FOnShieldDamage OnShieldDamage;

	// Sets shield to a specific value
	// The value cannot be negative
	UFUNCTION(Blueprintcallable, Category = "Shield")
		void SetShield(int shield);
	int GetMaxShield() const;
	void SetMaxShield(int val);

	// Adds a value to current shield value, can be a negative value
	void UpdateShield(int amount);
	// Completely restores shield
	void SetShieldToMax();

	void SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent);
	void UpdateStats();

	float GetReceivedDamageMult() const;
	void SetReceivedDamageMult(float val);

	// Call to break shield and disable regeneration for a specific amount of time
	void BreakShield();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		int m_maxShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float m_currentShieldAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float m_shieldRegenerationFac;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float m_shieldBrokenTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float m_reActivatedShieldStartFac;
	// Enables shield regeneration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		bool m_regenerateAble;
	float m_shieldRegenerationTime;
	float m_shieldTimeNoDamage; // Time it takes the shield to regenerate when taking no damage
	float m_receivedDamageMult;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
		class UActorStatsComponent* m_actorStatsComponent; // Pointer to fetch stats

	UPROPERTY()
		class UHealthComponent* m_healthComponent; // Pointer to health component to forward Damage to it when shield is 0

	// Regeneration time interval handle
	FTimerHandle m_shieldRegenerationTimerHandler;
	// Time after taking no damage
	FTimerHandle m_timeTakingNoShieldDamage;
	FTimerHandle m_shieldBrokenTimerHandle;

	bool m_canRegenerateShield;
	bool m_shieldBroken;

	void ResetRegenerationTimer();
	void ResetDamagedShieldTimer();
	// When shield is empty all damage is forwarded to health component if available
	void ForwardDamageToHealthComponent(int damage);	

	// Activates shield and sets shield to a specific start value
	void ActivateShield();
	// Enables shield 
	void EnableShield();
};
