// Fill out your copyright notice in the Description page of Project Settings.

#include "WeightedRandom.h"

WeightedRandom::WeightedRandom()
{ }

WeightedRandom::~WeightedRandom()
{ }

void WeightedRandom::SetProbabilities(TArray<float> probabilities)
{
	m_spawnStructs.SetNum(probabilities.Num());

	// insert probabilities & calculate sum
	float totalProbability = 0;
	for (int i = 0; i < probabilities.Num(); i++)
	{
		m_spawnStructs[i].m_probability = probabilities[i];
		totalProbability += probabilities[i];
	}

	// normalize probabilities & calculate thresholds
	float threshold = 0;
	float assertTotalProbability = 0; // assert final probability sum == 1
	if (m_spawnStructs.Num() < 1)
		assertTotalProbability = 1; // no items -> no probabilities
	for (int i = 0; i < m_spawnStructs.Num(); i++)
	{
		// normalize
		if (totalProbability == 0) // prevent divide-by-zero
			m_spawnStructs[i].m_probability = 1.f / m_spawnStructs.Num();
		else
			m_spawnStructs[i].m_probability /= totalProbability;

		// calc thresholds
		m_spawnStructs[i].thresholdMin = threshold;
		m_spawnStructs[i].thresholdMax = threshold += m_spawnStructs[i].m_probability;

		assertTotalProbability += m_spawnStructs[i].m_probability;
	}

	//UE_LOG(LogTemp, Error, TEXT("%f"), assertTotalProbability);
	check(assertTotalProbability > 0.99f && assertTotalProbability < 1.01f); // slight tolerance
}

int WeightedRandom::ReturnRandomIdx()
{
	// random float between 0 and 1
	float randomFloat = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	//UE_LOG(LogTemp, Display, TEXT("Random: %f"), randomFloat);

	for (int i = 0; i < m_spawnStructs.Num(); i++)
	{
		// check which range the random number falls into
		if (randomFloat >= m_spawnStructs[i].thresholdMin && randomFloat <= m_spawnStructs[i].thresholdMax)
			return i;
	}

	return -1;
}