#include "ElementStatusEffect.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ElementStatusEffectVictim.h"
#include "Kismet/GameplayStatics.h"
#include "ElementStatusEffectComponentBase.h"

// Sets default values
AElementStatusEffect::AElementStatusEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;

	m_particleEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	RootComponent = m_particleEffectComponent;

	m_effectDuration = 8.0f;
	m_currentElementStatusEffectImpactVal = 0;
	m_wantToBeActivated = false;
	m_activated = false;
}

void AElementStatusEffect::Activate()
{
	m_particleEffectComponent->ActivateSystem(true);

	m_activated = true;
	m_wantToBeActivated = false;
	GetWorld()->GetTimerManager().SetTimer(m_effectDurationHandle, this, &AElementStatusEffect::OnEndDuration, m_effectDuration, false);

	TArray<UActorComponent*> comps;
	GetComponents(comps);

	if (comps.Num() == m_elementStatusEffectStrenghts.Num())
	{
		int elementCompIdentifier = 0;
		for (int i = 0; i < comps.Num(); ++i)
		{
			UElementStatusEffectComponentBase* elementComp = Cast<UElementStatusEffectComponentBase>(comps[i]);
			if (elementComp)
			{
				m_elementComps.Add(elementComp);
				elementCompIdentifier = elementComp->GetIdentifier();
				if (elementCompIdentifier <= comps.Num())
				{
					elementComp->Init(m_victim, m_elementStatusEffectStrenghts[elementCompIdentifier], m_elementStatusEffectInterval);
					elementComp->ActivateComp();
				}
				else
					UE_LOG(LogTemp, Error, TEXT("ElementEffectComp: %s identifier out of range: %d"), *elementComp->GetName(), elementCompIdentifier);
				
			}
		}
	}
	else if (m_elementStatusEffectStrenghts.Num() < comps.Num() && m_elementStatusEffectStrenghts.Num() >= 1)
	{
		for (int i = 0; i < comps.Num(); ++i)
		{
			UElementStatusEffectComponentBase* elementComp = Cast<UElementStatusEffectComponentBase>(comps[i]);
			if (elementComp)
			{
				m_elementComps.Add(elementComp);

				elementComp->Init(m_victim, m_elementStatusEffectStrenghts[0], m_elementStatusEffectInterval);
				elementComp->ActivateComp();

				UE_LOG(LogTemp, Warning, TEXT("Not enough elementStatuseffectStrengths, using index 0 for all components"));

			}
		}
	}
}

void AElementStatusEffect::InitElementEffect(UWorld* world, ElementEffectType elementStatusEffectType, TArray<float> elementStatusEffectStrenghts, float elementStatusEffectInterval,
class IElementStatusEffectVictim* victim, int impact)
{
	m_elementStatusEffectType = elementStatusEffectType;
	m_elementStatusEffectStrenghts = elementStatusEffectStrenghts;
	m_elementStatusEffectInterval = elementStatusEffectInterval;
	m_victim = victim;
	m_maxElementStatusEffectImpactVal = m_victim->GetVictimMaxElementImpactVal();
	m_elementStatusEffectDepleteAmount = m_maxElementStatusEffectImpactVal * victim->GetVictimElementStatusEffectDepleteAmount();
	m_elementStatusEffectDepleteTime = victim->GetVictimElementStatusEffectDepleteTime();
	
	if (m_elementStatusEffectType == ElementEffectType::Ice || m_elementStatusEffectType == ElementEffectType::Thunder)
		m_effectDuration = m_elementStatusEffectStrenghts[0];

	AddImpact(impact);
}

void AElementStatusEffect::AddImpact(int val)
{
	m_currentElementStatusEffectImpactVal = FMath::Max(0, FMath::Min(m_currentElementStatusEffectImpactVal + val, m_maxElementStatusEffectImpactVal));

	GetWorld()->GetTimerManager().ClearTimer(m_elementEfectImpactReduceHandle);

	if (m_currentElementStatusEffectImpactVal >= m_maxElementStatusEffectImpactVal)
	{
		m_wantToBeActivated = true;
		return;
	}
	if (m_currentElementStatusEffectImpactVal <= 0)
	{
		if(m_victim)
			m_victim->RemoveStatusEffect(m_elementStatusEffectType);
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(m_elementEfectImpactReduceHandle, this, &AElementStatusEffect::DepleteElementEffectImpact, m_elementStatusEffectDepleteTime, false);

	UE_LOG(LogTemp, VeryVerbose, TEXT("Victim impact: %d // Victim max: %d"), m_currentElementStatusEffectImpactVal, m_maxElementStatusEffectImpactVal);
}

bool AElementStatusEffect::WantsToBeActivated()
{
	return m_wantToBeActivated;
}

bool AElementStatusEffect::IsActivated()
{
	return m_activated;
}

void AElementStatusEffect::DeactivateParticleSystem()
{
	m_particleEffectComponent->DeactivateSystem();
}

// Called when the game starts or when spawned
void AElementStatusEffect::BeginPlay()
{
	Super::BeginPlay();
	
	m_particleEffectComponent->DeactivateSystem();
}

// Called every frame
void AElementStatusEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AElementStatusEffect::DepleteElementEffectImpact()
{
	m_lock.Lock();
	AddImpact(-m_elementStatusEffectDepleteAmount);
	m_lock.Unlock();
}

void AElementStatusEffect::OnEndDuration()
{
	if (m_activated)
	{
		for (auto elementCompIt : m_elementComps)
		{

			elementCompIt->ShutdownComp();
		}
		if(m_victim)
			m_victim->RemoveStatusEffect(m_elementStatusEffectType);
	}
}

void AElementStatusEffect::EndPlay(EEndPlayReason::Type endPlayReason)
{
	UE_LOG(LogTemp, Display, TEXT("Status effect removed"));
}
