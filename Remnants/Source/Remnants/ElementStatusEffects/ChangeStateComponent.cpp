// Fill out your copyright notice in the Description page of Project Settings.

#include "ChangeStateComponent.h"
#include "ElementStatusEffectVictim.h"

void UChangeStateComponent::ActivateComp()
{
	m_victim->ChangeState(m_stateToChange);
}

void UChangeStateComponent::ShutdownComp()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset State"));
	m_victim->ResetState(m_stateToChange);
}

void UChangeStateComponent::BeginPlay()
{

}

void UChangeStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}



