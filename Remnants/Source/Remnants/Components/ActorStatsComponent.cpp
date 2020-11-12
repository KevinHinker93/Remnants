// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorStatsComponent.h"
#include "EventSystemHandlerSingleton.h"

// Sets default values for this component's properties
UActorStatsComponent::UActorStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_level = 1;
}

int UActorStatsComponent::GetLevel() const
{
	return m_level;
}

void UActorStatsComponent::SetLevel(int level)
{
	m_level = level;
	UE_LOG(LogTemp, Display, TEXT("Changed level to %d"), m_level);
	m_levelDirty = true;
}

//HP stat
int UActorStatsComponent::GetHPVal()
{
	return m_hpStat.CalcHPVal(m_level);
}

int UActorStatsComponent::GetPlayerElementEffectResistanceVal()
{
	return m_hpStat.CalcPlayerElementEffectResistanceVal(m_level);
}

int UActorStatsComponent::GetEnemyElementEffectResistanceVal()
{
	return m_hpStat.CalcEnemyElementEffectResistanceVal(m_level);
}

float UActorStatsComponent::GetElementEffectDepleteAmount()
{
	return m_hpStat.m_elementEffectDepleteAmount;
}

float UActorStatsComponent::GetElementEffectDepleteTime()
{
	return m_hpStat.CalcElementEffectDepleteTime(m_level);
}

//Shield stat
int UActorStatsComponent::GetShieldVal()
{
	return m_shieldStat.CalcShieldVal(m_level);
}

float UActorStatsComponent::GetShieldRegenAmount()
{
	return m_shieldStat.m_shieldRegenAmount;
}

float UActorStatsComponent::GetShieldRegenTime()
{
	return m_shieldStat.CalcShieldRegenTime(m_level);
}

//Damage stat
int UActorStatsComponent::GetDamageVal()
{
	return m_damageStat.CalcDamageVal(m_level);
}

float UActorStatsComponent::GetFiringSpeed()
{
	return m_damageStat.m_firingSpeed;
}

int UActorStatsComponent::GetMaxOverheatVal()
{
	return m_damageStat.CalcMaxOverheatVal(m_level);
}

int UActorStatsComponent::GetOverheatFac()
{
	return m_damageStat.CalcOverheatVal(m_level);
}

float UActorStatsComponent::GetOverheatRegenAmount()
{
	return m_damageStat.m_overheatRegenAmount;
}

float UActorStatsComponent::GetOverheatRegenTime()
{
	return m_damageStat.CalcOverheatRegenTime(m_level);
}

void UActorStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UActorStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (m_firstFrame)
	{
		m_firstFrame = false;
		return;
	}
	if (m_levelDirty)
	{
		bool check = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
		eventHandler->OnActorLevelChanged.Broadcast(this, m_level);
		m_levelDirty = false;
	}
}

