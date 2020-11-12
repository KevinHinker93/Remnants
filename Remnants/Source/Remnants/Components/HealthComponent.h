// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDamage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// Returns current health
	UFUNCTION(Blueprintcallable, Category = "Health")
		float GetHealth() const;
	UFUNCTION(Blueprintcallable, Category = "Health")
		float GetCurrentHealthPercentage() const;
	UPROPERTY(BlueprintAssignable, Category = "HealthDamage")
		FOnHealthDamage OnHealthDamage;
	int GetMaxHealth() const;
	// Sets health to a specific value
	// The value cannot be negative, so health parameter will always be an 
	// absolute value
	UFUNCTION(Blueprintcallable, Category = "Health")
		void SetHealth(int health);
	void SetMaxHealth(int val);
	// Adds a value to current amount of health, can be a negative value
	void UpdateHealth(int amount);
	// Completely restores health
	void SetHealthToMax();

	void SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent);
	void UpdateStats();

	void SetHealthRegeneratable(bool regeneratable);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		int m_maxHealth;
	// How much health will regen over time in percent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float m_healthRegenAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float m_healthRegenInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float m_currentHealth;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
		class UActorStatsComponent* m_actorStatsComponent; // Pointer to fetch stats	
	
	FTimerHandle m_healthRegenIntervalTimerHandle;

	bool m_canRegenHealth;
	void ZeroHealth();
	void ResetHealthRegen();
	void RegenHealth();
};
