// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../ElementStatusEffects/ElementEffectTypes.h"
#include "DamageEffectiveness.h"
#include "ElementAffinityComponent.generated.h"

class UAElement;

USTRUCT(BlueprintType)
struct FElementAffinityData
{
	GENERATED_USTRUCT_BODY()
	FElementAffinityData() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity Settings")
		ElementEffectType m_type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity Settings")
		float m_damageMult;
};

USTRUCT(BlueprintType)
struct FElementAffinityList
{
	GENERATED_USTRUCT_BODY()
		FElementAffinityList() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity Datas")
		TArray<FElementAffinityData> m_affinities;
};

UCLASS(Blueprintable, ClassGroup=(Element), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UElementAffinityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UElementAffinityComponent();

	// Request the element affinity component to set an affinity for the specified element
	// Will only set an affinity for the first element the component receives
	UFUNCTION(BlueprintCallable)
		void RequestElementAffinity(ElementEffectType type);

	UFUNCTION(BlueprintCallable)
		EDamageEffectiveness GetElementEffectiveness(ElementEffectType type, float& damageMult);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<int> GetElementWeaknesses();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity")
		TMap<ElementEffectType, int> m_elementItemIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity")
		TMap<ElementEffectType, FElementAffinityList> m_affinityList;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Holds the actual affinities: element weakness and immunity
	UPROPERTY()
		TMap<ElementEffectType, float> m_elementAffinities;

	bool m_affinityInitialized;

	UFUNCTION()
		void OnActorReceivedElement(AActor* actorReceivedElement, UAElement* element);

	EDamageEffectiveness CalcEffectivenessFromDamageMult(float damageMult);
};
