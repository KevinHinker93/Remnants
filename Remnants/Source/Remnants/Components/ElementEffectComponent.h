// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ElementStatusEffects/ElementEffectTypes.h"
#include "ElementEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UElementEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UElementEffectComponent();

	// If not immune to the status effect, it will be added to dictionary, but not activated. Resistance must be completely filled 
	// to activate the status effect
	void AddElementStatusEffect(ElementEffectType elementStatusEffectType, class AElementStatusEffect* elementStatusEffect);
	void RemoveElmentStatusEffect(ElementEffectType elementStatusEffectType);
	void RemoveAllElementStatusEffects();
	// Tells if a status effect already exists on the actor
	bool ElementStatusEffectExists(ElementEffectType elementStatusEffectType);
	bool ElementStatusEffectIsActivated(ElementEffectType elementStatusEffectType);
	// Returns true if resistance is completely filled
	bool ElementStatusEffectWantsToBeActivated(ElementEffectType elementStatusEffectType);
	void ActivateElementStatusEffect(ElementEffectType elementStatusEffectType);
	// Inflicts impact to resistance
	void AddImpactToElementStatusEffect(ElementEffectType elementStatusEffectType, int impact);

	void SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent);
	void UpdateStats(bool isPlayer);

	int GetElementStatusEffectResistanceVal() const;
	void SetElementStatusEffectResistanceVal(int elementStatusEffectResistanceVal);
	float GetElementStatusEffectDepleteAmount() const;
	float GetElementStatusEffectDepleteTime() const;

	float GetElementStatusEffectImpactFac() const;
	void SetElementStatusEffectImpactFac(float elementStatusEffectImpactFac);

protected:
	int m_maxElementStatusEffects;
	int m_ElementStatusEffectResistanceVal;
	float m_ElementStatusEffectDepleteAmount;
	float m_ElementStatusEffectDepleteTime;
	float m_elementStatusEffectImpactFac;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FCriticalSection m_criticalSection;

	UPROPERTY()
		class UActorStatsComponent* m_actorStatsComponent; // Pointer to fetch stats

	UPROPERTY()
		TMap<ElementEffectType, class AElementStatusEffect*> m_elementStatusEffects; // List of all status effects on enemy/player
};
