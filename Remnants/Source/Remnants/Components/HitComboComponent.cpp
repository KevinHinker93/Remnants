#include "HitComboComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../EventSystemHandlerSingleton.h"
#include "HealthComponent.h"
#include "ShieldComponent.h"


UHitComboComponent::UHitComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	m_maxComboDamageMult = 2.0f;
	m_maxComboHitCount = 200;
	m_maxComboDuration = 6.0f;
	m_maxComboHealthLoss = 0.7f;
	m_comboHealthLossDifficultyChange = 0.1f;
	m_comboDifficultyStep = 0.25f;
	m_currentDifficultyStep = m_maxComboHitCount * m_comboDifficultyStep;
}

void UHitComboComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* owner = GetOwner();
	m_healthCompOfOwner = Cast<UHealthComponent>(owner->GetComponentByClass(UHealthComponent::StaticClass()));
	m_shieldCompOfOwner = Cast<UShieldComponent>(owner->GetComponentByClass(UShieldComponent::StaticClass()));

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnPlayerDealtDamage.AddDynamic(this, &UHitComboComponent::InflictedDamage);

	EndCombo();
}

void UHitComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHitComboComponent::InflictedDamage(float damage)
{
	ClearComboTimer();

	m_currentAccumulatedDamage += damage;
	++m_currentHitCount;

	m_currentDamageMultiplier = static_cast<float>(m_currentHitCount) / m_maxComboHitCount;
	m_currentDamageMultiplier += 1.0f;

	m_currentDamageMultiplier = FMath::Clamp(m_currentDamageMultiplier, 0.0f, m_maxComboDamageMult);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnComboDataChanged.Broadcast(m_currentAccumulatedDamage, m_currentHitCount, m_currentDamageMultiplier);

	m_currentDifficultyLvl = m_currentHitCount / m_currentDifficultyStep;
	m_currentComboDuration = m_maxComboDuration - m_currentDifficultyLvl;

	GetWorld()->GetTimerManager().SetTimer(m_comboTimerHandle, this, &UHitComboComponent::EndCombo, m_currentComboDuration, false);
}

void UHitComboComponent::WasDamaged(float damage)
{
	float maxHealthIndicator = 1000.0f;
	if (m_healthCompOfOwner && m_shieldCompOfOwner)
		maxHealthIndicator = static_cast<float>((m_healthCompOfOwner->GetMaxHealth() + m_shieldCompOfOwner->GetMaxShield())) / 2.0f;

	m_currentReceivedAccumulatedDamage += damage;
	m_currentComboHealthLoss = m_maxComboHealthLoss - (m_comboHealthLossDifficultyChange * m_currentDifficultyLvl);

	if (m_currentReceivedAccumulatedDamage / maxHealthIndicator >= m_currentComboHealthLoss)
		EndCombo();
}

void UHitComboComponent::ClearComboTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(m_comboTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(m_comboTimerHandle);
}

void UHitComboComponent::EndCombo()
{
	ClearComboTimer();

	m_currentAccumulatedDamage = 0.0f;
	m_currentDamageMultiplier = 1.0f;
	m_currentHitCount = 0;
	m_currentReceivedAccumulatedDamage = 0.0f;

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnComboEnded.Broadcast();
}