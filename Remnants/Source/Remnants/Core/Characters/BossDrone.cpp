// Fill out your copyright notice in the Description page of Project Settings.

#include "BossDrone.h"
#include "Components/HealthComponent.h"
#include "Components/ShieldComponent.h"
#include "ElementDamageType.h"
#include "EventSystemHandlerSingleton.h"

void ABossDrone::Kill()
{
	if (m_isDead)
		return;

	Super::Kill();
	
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnBossDied.Broadcast();
	eventHandler->OnBossDropItems.Broadcast(m_directReceivedItemIdsAfterDeath, m_droppedItemAmount);
}

void ABossDrone::RemoveElementShieldFromMap(FName elementName)
{
	m_elementShields.Remove(elementName);
}

void ABossDrone::AddBrokenShield(FName elementName)
{
	if(!m_brokenShields.Contains(elementName))
		m_brokenShields.Emplace(elementName);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnBossShieldBroken.Broadcast(elementName.ToString());
}

float ABossDrone::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (!m_bossActivated)
	{
		m_bossActivated = true;
		eventHandler->OnBossHealthChanged.Broadcast(m_healthComponent->GetCurrentHealthPercentage());
		eventHandler->OnBossShieldChanged.Broadcast("All", m_shieldComponent->GetCurrentHealthPercentage());
		eventHandler->OnBossActivated.Broadcast(true);
		ActivateBoss();
	}

	UElementDamageType* damageType = Cast<UElementDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	float actualDamage = CalcActualDamage(damageType, Damage);
	if (damageType)
	{
		ElementEffectType elementType = damageType->GetElementType();
		m_lastHittedElement = elementType;
	}
	if (IsDamagingBossHealth(damageType, actualDamage))
	{
		eventHandler->OnPlayerDealtDamage.Broadcast(actualDamage);
		return actualDamage;
	}

	if (IsAbsorbingDamage(damageType))
	{
		if (damageType)
		{
			FName elementName = damageType->GetElementTypeName();
			if (m_brokenShields.Contains(elementName))
				eventHandler->OnRequestHintDisplay.Broadcast("Boss2");
			else
				eventHandler->OnRequestHintDisplay.Broadcast("Boss");
		}
		else
		{
			eventHandler->OnRequestHintDisplay.Broadcast("Boss");
		}
		return 0;
	}
	
	DamageElementalShield(actualDamage, damageType, DamageCauser);
	eventHandler->OnPlayerDealtDamage.Broadcast(actualDamage);
	return actualDamage;
}

void ABossDrone::BeginPlay()
{
	Super::BeginPlay();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnKeyHolderKill.AddDynamic(this, &ABossDrone::OnKeyHolderKill);
	eventHandler->OnActivateBossWBrokenShields.AddDynamic(this, &ABossDrone::ActivateBossWithBrokenShields);

	// Fetch all shieldComponents and setup their element accessor via tag, for the damage logic
	TArray<UActorComponent*> shieldComponents = GetComponentsByClass(UShieldComponent::StaticClass());
	if (shieldComponents.Num() < m_requiredElementShieldCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss drone has not enough shield components: required count %d // current count: %d "), m_requiredElementShieldCount, shieldComponents.Num());
		return;
	}

	for (auto shieldComponent : shieldComponents)
	{
		if (shieldComponent->ComponentTags.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Boss shield component has no tag, have you forgot to set it?"));
			return;
		}
		UShieldComponent* shieldComponentCasted = Cast<UShieldComponent>(shieldComponent);
		m_elementShields.Emplace(shieldComponent->ComponentTags[0], shieldComponentCasted);
		shieldComponentCasted->SetActorStatsComponent(m_actorStatsComponent);
		shieldComponentCasted->UpdateStats();
		shieldComponentCasted->SetShieldToMax();
	}
	m_bossActivated = false;
	m_destroyOnKill = false;

	SetFloatVal(EntityStats::FiringStrat, 0.0f);
}

bool ABossDrone::IsAbsorbingDamage(UElementDamageType* damageType)
{

	if (damageType)
	{
		FName elementName = damageType->GetElementTypeName();
		// Absorb damage if boss was not hit with right element
		if (!m_elementShields.Contains(elementName))
		{
			UE_LOG(LogTemp, Verbose, TEXT("Boss absorbs %s elemental damage"), *elementName.ToString());
			AbsorbDamage();
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("Boss absorbs non elemental damage"));
		// Also absorb non elemental damage
		AbsorbDamage();
		return true;
	}

	return false;
}

bool ABossDrone::IsDamagingBossHealth(UElementDamageType* damageType, float damage)
{
	if (m_elementShields.Num() == 0)
	{
		OnAnyBossDamage(nullptr, m_shieldComponent, -damage);
		m_healthComponent->UpdateHealth(-damage);
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnBossHealthChanged.Broadcast(m_healthComponent->GetCurrentHealthPercentage());
		return true;
	}
	else
		return false;

	return false;
}

UShieldComponent* ABossDrone::GetElementalShieldToDamage(FName elementName)
{
	return m_elementShields[elementName];		
}

void ABossDrone::DamageElementalShield(float damage, UElementDamageType* damageType, AActor* DamageCauser)
{
	if (damageType)
	{
		
		m_tagOfLastDamageCauser = DamageCauser->Tags[0];
		FName elementName = damageType->GetElementTypeName();

		UShieldComponent* shieldComponentToDamage = GetElementalShieldToDamage(elementName);
		OnAnyBossDamage(shieldComponentToDamage, m_shieldComponent, -damage);
		if(shieldComponentToDamage)
			shieldComponentToDamage->UpdateShield(-damage);

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnBossShieldChanged.Broadcast(elementName, shieldComponentToDamage->GetCurrentHealthPercentage());
	}
}

float ABossDrone::CalcActualDamage(UElementDamageType* damageType, float initialDamage)
{
	if (damageType)
		{
			int randSeed = FMath::Rand();
			FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
			float randomMult = FMath::RandRange(damageType->GetRandomDamageMultRange().m_randomDamageMultMin, damageType->GetRandomDamageMultRange().m_randomDamageMultMax);
			return initialDamage * randomMult;
		}
		else
		{
			int randSeed = FMath::Rand();
			FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
			float randomMult = FMath::RandRange(0.9f, 1.1f);
			return initialDamage * randomMult;
		}
}

void ABossDrone::ActivateBossWithBrokenShields(const TSet<FString> brokenShields)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (!m_bossActivated)
	{
		m_bossActivated = true;
		eventHandler->OnBossHealthChanged.Broadcast(m_healthComponent->GetCurrentHealthPercentage());

		for (auto activeShieldIt : m_elementShields)
		{
			UShieldComponent* shieldToMax = activeShieldIt.Value;
			eventHandler->OnBossShieldChanged.Broadcast(activeShieldIt.Key, shieldToMax->GetCurrentHealthPercentage());
		}

		for (auto shieldNameIt : brokenShields)
		{
			FName shieldName = FName(*shieldNameIt);
			UShieldComponent* shieldToBreak = m_elementShields[shieldName];
			shieldToBreak->BreakShield();
		}

		eventHandler->OnBossActivated.Broadcast(false);
		ActivateBoss();
	}
}

void ABossDrone::OnKeyHolderKill(int keyItemId)
{
	// increase level maybe?
	/*if (keyItemId < 0)
		Destroy();*/
}