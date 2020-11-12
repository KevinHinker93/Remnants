// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageArea.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "../../ElementDamageType.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/SceneComponent.h"
#include "../../EventSystemHandlerSingleton.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADamageArea::ADamageArea()
{
	PrimaryActorTick.bCanEverTick = false;

	m_damageAreaCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	m_damageAreaCollider->InitSphereRadius(1.0f);
	m_damageAreaCollider->BodyInstance.SetCollisionProfileName("Projectile");

	m_damageAreaCollider->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_damageAreaCollider->CanCharacterStepUpOn = ECB_No;

	RootComponent = m_damageAreaCollider;

	m_particleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	m_particleComponent->SetupAttachment(RootComponent);

	m_areaDamages = true;
	m_destroyOnAttachedDestroyed = false;
}

void ADamageArea::InitDamageArea(float damage, float interval, AController* eventInstigator, AActor* damageCauser,
		TSubclassOf<UElementDamageType> damageTypeClass, FName shooterType)
{
	m_damage = damage;
	m_damageInterval = interval;
	m_eventInstigator = eventInstigator;
	m_damageCauser = damageCauser;
	m_damageTypeClass = damageTypeClass;
	m_lifeTime = GetLifeSpan();

	if (m_areaDamages)
	{
		// Change collsion preset
		FString profileName = shooterType.ToString() + m_collisionPostfix.ToString(); // Shootertype is typically tag of shooter
		FName collisionProfileName = FName(*profileName);
		m_damageAreaCollider->BodyInstance.SetCollisionProfileName(collisionProfileName);
	}
}

void ADamageArea::OnAreaAttached()
{
	if (m_destroyOnAttachedDestroyed)
	{
		USceneComponent* attachParent = RootComponent->GetAttachParent();
		if (!attachParent)
			return;
		AActor* attachedToActor = attachParent->GetOwner();
		if (attachedToActor && attachedToActor->ActorHasTag("Enemy"))
			attachedToActor->OnDestroyed.AddDynamic(this, &ADamageArea::OnAttachedActorDestroyed);
	}
}

// Called when the game starts or when spawned
void ADamageArea::BeginPlay()
{
	Super::BeginPlay();

	if(m_areaDamages)
		GetWorld()->GetTimerManager().SetTimer(m_damageTimerHandle, this, &ADamageArea::OnDamage, m_damageInterval, true);
}

// Called every frame
void ADamageArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageArea::OnDamage()
{
	TArray<AActor*> overlappingActors;
	m_damageAreaCollider->GetOverlappingActors(overlappingActors);

	FVector emptyHitFromDirection = FVector::ZeroVector;
	FHitResult emptyHitResult;

	for(int i = 0; i < overlappingActors.Num(); i++)
		UGameplayStatics::ApplyPointDamage(overlappingActors[i], m_damage * m_damageMult, emptyHitFromDirection, emptyHitResult, NULL, m_damageCauser, m_damageTypeClass);
}

void ADamageArea::OnAttachedActorDestroyed(AActor* destroyedActor)
{
	OnKillAnimation();
	//Destroy();
}