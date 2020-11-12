// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElementStatusEffectComponentBase.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), ABSTRACT)
class REMNANTS_API UElementStatusEffectComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UElementStatusEffectComponentBase();
	void Init(class IElementStatusEffectVictim* victim, float strength, float interval);
	int GetIdentifier() const;
	virtual void ActivateComp() { check(0 && "You must override this"); }
	virtual void ShutdownComp() { check(0 && "You must override this"); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Identifier")
		int m_effectIdentifier;

	class IElementStatusEffectVictim* m_victim;
	float m_elementStatusEffectStrength;	
	float m_elementStatusEffectInterval;
};
