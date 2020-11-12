// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"
#include "ElementStatusEffectVictim.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

void UBuff::ActivateBuff(AActor* actorToBuff)
{
	if (!actorToBuff)
		return;
	IElementStatusEffectVictim* victim = Cast<IElementStatusEffectVictim>(actorToBuff);
	if (victim)
		victim->SetFloatVal(m_statToManipulate, m_buffStrength);
}

FName UBuff::GetBuffName() const
{
	return m_buffName;
}

