// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnStruct.h"

class REMNANTS_API WeightedRandom
{
public:
	WeightedRandom();
	virtual ~WeightedRandom();

	void SetProbabilities(TArray<float> probabilities);
	virtual int ReturnRandomIdx();

protected:
	TArray<FSpawnStruct> m_spawnStructs;
};
