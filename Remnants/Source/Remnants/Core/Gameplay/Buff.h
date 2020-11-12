// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ElementStatusEffects/StatsEnum.h"
#include "Buff.generated.h"

class IElementStatusEffectVictim;

UCLASS(Blueprintable)
class REMNANTS_API UBuff : public UObject
{
	GENERATED_BODY()

public:
	void ActivateBuff(AActor* actorToBuff);
	FName GetBuffName() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buff)
		FName m_buffName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buff)
		float m_buffStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manipulated Stat")
		EntityStats m_statToManipulate;
	
};
