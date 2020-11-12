// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageOverTimeComponent.h"
#include "../Components/ShieldComponent.h"
#include "ElementStatusEffectVictim.h"

void UDamageOverTimeComponent::ActivateComp()
{
	m_victimMaxHealth = m_victim->GetVictimMaxHealth();
	m_victimShieldComponent = m_victim->GetVictimShieldComponent();

	GetWorld()->GetTimerManager().SetTimer(m_damageInvervalHandle, this, &UDamageOverTimeComponent::OnDamageVictim, m_elementStatusEffectInterval, false);
}

void UDamageOverTimeComponent::ShutdownComp()
{
	UE_LOG(LogTemp, Warning, TEXT("Status effect removed"));
}

void UDamageOverTimeComponent::BeginPlay()
{

}

void UDamageOverTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

void UDamageOverTimeComponent::OnDamageVictim()
{
	DamageVictim();
}

void UDamageOverTimeComponent::DamageVictim()
{
	int Damage = m_victimMaxHealth * m_elementStatusEffectStrength;
	if (m_victimShieldComponent)
		m_victimShieldComponent->UpdateShield(-Damage);

	GetWorld()->GetTimerManager().ClearTimer(m_damageInvervalHandle);
	GetWorld()->GetTimerManager().SetTimer(m_damageInvervalHandle, this, &UDamageOverTimeComponent::OnDamageVictim, m_elementStatusEffectInterval, false);
}


