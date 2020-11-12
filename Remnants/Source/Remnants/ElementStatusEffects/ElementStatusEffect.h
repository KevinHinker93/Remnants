// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementEffectTypes.h"
#include "ElementStatusEffect.generated.h"

UCLASS()
class REMNANTS_API AElementStatusEffect : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AElementStatusEffect();

	// Inflictes the status effect
	void Activate();

	// When entity gets hit the first time with a status effect, the effect will be initialized and added to the element effect component
	void InitElementEffect(UWorld* world, ElementEffectType elementStatusEffectType, TArray<float> elementStatusEffectStrenghts, float elementStatusEffectInterval,
			class IElementStatusEffectVictim* victim, int impact);
	// Adds impact after every hit
	void AddImpact(int val);
	// If someone has enough impact then the component will mark this effect as wants to be activated
	bool WantsToBeActivated();
	bool IsActivated();
	void DeactivateParticleSystem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Eff Duration")
		float m_effectDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Eff Particle")
		class UParticleSystemComponent* m_particleEffectComponent;

	UPROPERTY()
		TArray<float> m_elementStatusEffectStrenghts;

	class IElementStatusEffectVictim* m_victim;

	FTimerHandle m_effectDurationHandle;
	FTimerHandle m_elementEfectImpactReduceHandle;
	FCriticalSection m_lock;

	ElementEffectType m_elementStatusEffectType;
	float m_elementStatusEffectInterval;
	int m_maxElementStatusEffectImpactVal;
	int m_currentElementStatusEffectImpactVal;
	float m_elementStatusEffectDepleteAmount;
	float m_elementStatusEffectDepleteTime;
	bool m_wantToBeActivated;
	bool m_activated;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
		TArray<class UElementStatusEffectComponentBase*> m_elementComps;

	void DepleteElementEffectImpact();	
	void OnEndDuration();

	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;
};
