// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ElementStatusEffects/ElementEffectTypes.h"
#include "ElementDamageType.generated.h"

USTRUCT(BlueprintType)
struct FRandomDamageMultRange
{
	GENERATED_USTRUCT_BODY()
		FRandomDamageMultRange() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomDamageMult")
		float m_randomDamageMultMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomDamageMult")
		float m_randomDamageMultMax;
};


UCLASS()
class REMNANTS_API UElementDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	TSubclassOf<class AElementStatusEffect> GetStatusEffectClass() const;
	FName GetElementTypeName() const;
	ElementEffectType GetElementType() const;
	int GetElementLevel() const;
	// How much elemental impact from inflicted damage will be added
	float GetElementStatusEffectImpactMult() const;
	// Every element has a base effect value, that will be added to element resistance, when beeing hit
	float GetElementStatusEffectBaseImpact() const;
	// Returns influence of the effect, meaning how much a stat will be reduced e.g.
	TArray<float> GetElementStatusEffectStrenghts() const;

	float GetElementStatusEffectInterval() const;

	FRandomDamageMultRange GetRandomDamageMultRange();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effect")
		TSubclassOf<class AElementStatusEffect> m_ElementStatusEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		FName m_elementTypeName; // For Debug info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		ElementEffectType m_elementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		int m_elementLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		float m_elementStatusEffectImpactMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		int m_elementStatusEffectBaseImpact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		TArray<float> m_elementStatusEffectStrenghts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
		float m_elementStatusEffectInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageTypeRandomDamage")
		FRandomDamageMultRange m_randomDamageMultRange;
};
