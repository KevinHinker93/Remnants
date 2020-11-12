// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldComponent.h"
#include "HealthComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "EventSystemHandlerSingleton.h"
#include "Engine/World.h"
#include "ActorStatsComponent.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

		m_maxShield = 1000;
		m_shieldRegenerationTime = 0.7f;
		m_shieldTimeNoDamage = 4.0f;
	    m_shieldRegenerationFac = 50;
		//m_currentShieldAmount = m_maxShield;
		m_canRegenerateShield = false;
		m_receivedDamageMult = 1.0f;
		m_shieldBroken = false;
		m_regenerateAble = true;
}

// Returns current shield
float UShieldComponent::GetShield() const
{
	return m_currentShieldAmount;
}

float UShieldComponent::GetCurrentHealthPercentage() const
{
	return m_currentShieldAmount / m_maxShield;
}

// Sets shield to a specific value
// The value cannot be negative
void UShieldComponent::SetShield(int shield)
{
	if (shield < 0)
		UE_LOG(LogTemp, Warning, TEXT("Cannot set shield val to a negative val"));

	m_currentShieldAmount = FMath::Clamp(FMath::Abs(shield), 0, m_maxShield);

	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnShieldChanged.Broadcast(this, (float)GetShield() / (float)GetMaxShield());
}

int UShieldComponent::GetMaxShield() const
{
	return m_maxShield;
}

// Adds a value to current shield value, can be a negative value
void UShieldComponent::UpdateShield(int amount)
{
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	//eventHandler->OnReceiveDamage.Broadcast(this, amount);
		
	if (amount < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(m_timeTakingNoShieldDamage);
		GetWorld()->GetTimerManager().ClearTimer(m_shieldRegenerationTimerHandler);
		GetWorld()->GetTimerManager().SetTimer(m_timeTakingNoShieldDamage, this, &UShieldComponent::ResetDamagedShieldTimer, m_shieldTimeNoDamage, false);
		m_canRegenerateShield = false;
	}

	if(m_shieldBroken)
		ForwardDamageToHealthComponent(amount);

	m_currentShieldAmount = FMath::Clamp((int)m_currentShieldAmount + amount, 0, m_maxShield);

	if (amount < 0 && m_currentShieldAmount <= 0 && !m_shieldBroken)
		BreakShield();
	else if(amount < 0 && !m_shieldBroken)
		OnShieldDamage.Broadcast();

	eventHandler->OnShieldChanged.Broadcast(this, (float)GetShield() / (float)GetMaxShield());
	UE_LOG(LogTemp, VeryVerbose, TEXT("Actor: %s current Shield: %f // max shield: %d"), *GetOwner()->GetName(), GetShield(), m_maxShield);
}

// Completely restores shield
void UShieldComponent::SetShieldToMax()
{
	m_currentShieldAmount = m_maxShield;
	ResetRegenerationTimer();
	ResetDamagedShieldTimer();
	EnableShield();
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnShieldChanged.Broadcast(this, (float)GetShield() / (float)GetMaxShield());
}

// Increases maximum shield, cannot increase max shield with a negative value,
// so amount parameter will alway be an absolute value
void UShieldComponent::SetMaxShield(int val)
{
	m_maxShield = FMath::Abs(val);
	if (m_currentShieldAmount > m_maxShield)
		m_currentShieldAmount = m_maxShield;
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnMaxShieldChanged.Broadcast(this, m_maxShield);
}

void UShieldComponent::SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent)
{
	m_actorStatsComponent = actorStatsComponent;
}

void UShieldComponent::UpdateStats()
{
	m_maxShield = m_actorStatsComponent->GetShieldVal();
	SetShield(FMath::Min(int(m_currentShieldAmount), m_maxShield));
	//SetShieldToMax();
	m_shieldRegenerationFac = m_actorStatsComponent->GetShieldRegenAmount();
	m_shieldRegenerationTime = m_actorStatsComponent->GetShieldRegenTime();
}

float UShieldComponent::GetReceivedDamageMult() const
{
	return m_receivedDamageMult;
}

void UShieldComponent::SetReceivedDamageMult(float val)
{
	m_receivedDamageMult = val;
}

// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* parentActor = GetOwner();

	TArray<UActorComponent*> comps;
	parentActor->GetComponents(comps);
	UHealthComponent* healthComp = 0;
	bool foundComponent = false;
	for (int i = 0; i < comps.Num(); ++i)
	{
		healthComp = Cast<UHealthComponent>(comps[i]);
		if (healthComp)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Assigned Health comp to shield comp"));
			m_healthComponent = healthComp;
			foundComponent = true;
			break;
		}
	}
	if (!healthComp && !foundComponent)
		UE_LOG(LogTemp, Error, TEXT("Shield found no Health component"));
}

// Called every frame
void UShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_currentShieldAmount < m_maxShield && m_canRegenerateShield && !m_shieldBroken && m_regenerateAble)
	{
		float shieldRegenAmount = float(m_maxShield * m_shieldRegenerationFac);
		UE_LOG(LogTemp, Verbose, TEXT("shield regen %f"), shieldRegenAmount);
		UpdateShield(shieldRegenAmount);
		m_canRegenerateShield = false;
		GetWorld()->GetTimerManager().SetTimer(m_shieldRegenerationTimerHandler, this, &UShieldComponent::ResetRegenerationTimer, m_shieldRegenerationTime, false);
	}
}

void UShieldComponent::ResetRegenerationTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(m_shieldRegenerationTimerHandler);
	m_canRegenerateShield = true;
}

void UShieldComponent::ResetDamagedShieldTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(m_timeTakingNoShieldDamage);
	m_canRegenerateShield = true;
}

// When shield is empty all damage is forwarded to health component if available
void UShieldComponent::ForwardDamageToHealthComponent(int damage)
{
	if (m_healthComponent)
		m_healthComponent->UpdateHealth(damage);
	else
		UE_LOG(LogTemp, Error, TEXT("No health component to deal damage to"));
}

void UShieldComponent::BreakShield()
{
	m_shieldBroken = true;
	// Shield is broken for a specific amount of time, after timer finishes the shield will be activated
	GetWorld()->GetTimerManager().SetTimer(m_shieldBrokenTimerHandle, this, &UShieldComponent::ActivateShield, m_shieldBrokenTime, false);
	OnShieldBreak.Broadcast();
}

void UShieldComponent::ActivateShield()
{
	EnableShield();

	float shieldRegenAmount = float(m_maxShield * m_reActivatedShieldStartFac);
	UpdateShield(shieldRegenAmount);
	m_canRegenerateShield = false;
	GetWorld()->GetTimerManager().SetTimer(m_shieldRegenerationTimerHandler, this, &UShieldComponent::ResetRegenerationTimer, m_shieldRegenerationTime, false);
}

void UShieldComponent::EnableShield()
{
	GetWorld()->GetTimerManager().ClearTimer(m_shieldBrokenTimerHandle);
	if (m_shieldBroken)
	{
		m_shieldBroken = false;
		OnActivateShield.Broadcast();
	}
	
	ResetDamagedShieldTimer();
	ResetRegenerationTimer();
}