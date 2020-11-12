// Fill out your copyright notice in the Description page of Project Settings.

#include "ManipulateStatComponent.h"
#include "ElementStatusEffectVictim.h"

void UManipulateStatComponent::ActivateComp()
{
	m_victim->SetFloatVal(m_statToManipulate, 1 + m_elementStatusEffectStrength);
}

void UManipulateStatComponent::ShutdownComp()
{
	UE_LOG(LogTemp, Warning, TEXT("Status effect removed"));
	m_victim->ResetFloatVal(m_statToManipulate, 1 + m_elementStatusEffectStrength);
}

void UManipulateStatComponent::BeginPlay()
{

}

void UManipulateStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}


