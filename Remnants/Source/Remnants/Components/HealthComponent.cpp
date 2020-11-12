// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "ActorStatsComponent.h"
#include "EventSystemHandlerSingleton.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../KillAble.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_maxHealth = 1000;
	m_currentHealth = m_maxHealth;
	m_healthRegenAmount = 0.05f;
	m_healthRegenInterval = 0.2f;
	m_canRegenHealth = false;
}

// Returns current health
float UHealthComponent::GetHealth() const
{
	return m_currentHealth;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return m_currentHealth / m_maxHealth;
}

int UHealthComponent::GetMaxHealth() const
{
	return m_maxHealth;
}

// Sets health to a specific value
// The value cannot be negative, so health parameter will always be an 
// absolute value
void UHealthComponent::SetHealth(int health)
{
	if (health < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot set health val to a negative val, using absolute val"));
	}
	m_currentHealth = FMath::Clamp(FMath::Abs(health), 0, m_maxHealth);
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnHealthChanged.Broadcast(this, (float)GetHealth() / (float)GetMaxHealth());
}

// Increases maximum health, cannot increase max health with a negative value,
// so amount parameter will alway be an absolute value
void UHealthComponent::SetMaxHealth(int val)
{
	m_maxHealth = FMath::Abs(val);
	if (m_currentHealth > m_maxHealth)
		m_currentHealth = m_maxHealth;
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnMaxHealthChanged.Broadcast(this, m_maxHealth);
}

// Adds a value to current amount of health, can be a negative value
void UHealthComponent::UpdateHealth(int amount)
{
	m_currentHealth = FMath::Clamp((int)m_currentHealth + amount, 0, m_maxHealth);
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnHealthChanged.Broadcast(this, (float)GetHealth()/(float)GetMaxHealth());
	UE_LOG(LogTemp, Verbose, TEXT("Actor: %s current Health: %f"), *GetOwner()->GetName(), m_currentHealth);

	if (amount < 0)
		OnHealthDamage.Broadcast();

	if (m_currentHealth <= 0)
		ZeroHealth();

	if(amount <= 0)
		ResetHealthRegen();
}

// Completely restores health
void UHealthComponent::SetHealthToMax()
{
	m_currentHealth = m_maxHealth;
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnHealthChanged.Broadcast(this, (float)GetHealth() / (float)GetMaxHealth());
}

void UHealthComponent::SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent)
{
	m_actorStatsComponent = actorStatsComponent;
}

void UHealthComponent::UpdateStats()
{
	m_maxHealth = m_actorStatsComponent->GetHPVal();
	SetHealth(FMath::Min(int(m_currentHealth), m_maxHealth));

	//SetHealthToMax();
	bool check = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(check);
	eventHandler->OnMaxHealthChanged.Broadcast(this, m_maxHealth);
}

void UHealthComponent::SetHealthRegeneratable(bool regeneratable)
{
	m_canRegenHealth = regeneratable;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/*if (m_currentHealth <= 0)
	{
		ZeroHealth();
	}*/
}

void UHealthComponent::ZeroHealth()
{
	auto parentActorKillAble = Cast<IKillAble>(GetOwner());
	if (parentActorKillAble)
	{
		//FName actorToKillName = parentActorKillAble->GetName();
		parentActorKillAble->Kill();
	}
	else
		GetOwner()->Destroy();
}

void UHealthComponent::ResetHealthRegen()
{
	if (m_canRegenHealth)
	{
		GetWorld()->GetTimerManager().ClearTimer(m_healthRegenIntervalTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(m_healthRegenIntervalTimerHandle, this, &UHealthComponent::RegenHealth, m_healthRegenInterval, true);
	}
}

void UHealthComponent::RegenHealth()
{
	if(m_currentHealth < m_maxHealth)
		UpdateHealth(m_maxHealth * m_healthRegenAmount);
}