// Fill out your copyright notice in the Description page of Project Settings.

#include "LootCrate.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "../../EventSystemHandlerSingleton.h"

ALootCrate::ALootCrate()
{
	PrimaryActorTick.bCanEverTick = false;
}

float ALootCrate::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Kill();

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ALootCrate::BeginPlay()
{
	Super::BeginPlay();

	m_items = NewObject<UItemCollection>();
	m_items->SetMaxSlotCount(m_itemCountMax);

	m_collider = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	if (m_collider)
	{
		m_collider->OnComponentHit.AddDynamic(this, &ALootCrate::OnHit);
		m_collider->OnComponentBeginOverlap.AddDynamic(this, &ALootCrate::OnOverlapBegin);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Loot Crate does not have a box collider"));

	eventHandler->OnCrateDestructionRequest.AddDynamic(this, &ALootCrate::OnCrateDestructionRequest);
	
	m_weightedRandom.SetProbabilities(m_itemProbabilities);
	
	GenerateInventory();
}

void ALootCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootCrate::GenerateInventory()
{
	if (m_itemCountMax == 0 || m_itemCountMax < m_itemCountMin)
		return;

	// get random number of items
	int itemCount = FMath::RandRange(m_itemCountMin, m_itemCountMax);

	// add items to inventory (also stacks items automatically)
	for (int i = 0; i < itemCount; i++)
		GenerateItem();

	UE_LOG(LogTemp, Display, TEXT("Loot crate generated %d item stacks:"), m_items->GetSlotCount());
	m_items->Print();
}

void ALootCrate::ReleaseInventory()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	// place items in world
	for (int i = 0; i < m_items->GetSlotCount(); i++)
	{
		float offsetX = FMath::FRandRange(-m_itemSpread, m_itemSpread);
		float offsetY = FMath::FRandRange(-m_itemSpread, m_itemSpread);
		UE_LOG(LogTemp, Display, TEXT("Loot crate spawned item with offset %f %f"), offsetX, offsetY);

		FTransform transform = GetActorTransform();
		transform.AddToTranslation(FVector(offsetX, offsetY, 0));

		eventHandler->OnPickupSpawnRequest.Broadcast(this, m_items->GetStack(i), transform, GetLevel());
	}
}

void ALootCrate::GenerateItem()
{
	int idx = m_weightedRandom.ReturnRandomIdx();

	if (idx >= 0 && idx < m_possibleItemIds.Num())
	{
		int itemId = m_possibleItemIds[idx];

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		// add to inventory
		m_items->AddItem(itemId);
	}
}

void ALootCrate::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	m_criticalSection.Lock();
	if (IsPendingKill())
		return;
	OnLootableObjectDestroy();
	Kill();
	m_criticalSection.Unlock();
}

void ALootCrate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	m_criticalSection.Lock();
	if (IsPendingKill())
		return;
	OnLootableObjectDestroy();
	Kill();
	m_criticalSection.Unlock();
}

void ALootCrate::Kill()
{
	// notify spawn point that crate has been destroyed
	for (auto tag : Super::Tags)
	{
		if (tag.ToString().Contains("Spawn Group: "))
		{
			bool checkSingleton = false;
			auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
			eventHandler->OnSpawnpointObjectDestroyed.Broadcast(tag.ToString());
			break;
		}
	}

	// spawn death particle system
	auto particleSystem = UGameplayStatics::SpawnEmitterAtLocation(this, m_deathParticleSystem, GetActorLocation());

	if (m_save)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnCrateDestroyed.Broadcast(GetName());
	}

	ReleaseInventory();
	Destroy();
}

void ALootCrate::OnCrateDestructionRequest(FString crateName)
{
	if (GetName() == crateName)
	{
		UE_LOG(LogTemp, Display, TEXT("Destroyed crate %s due to save game"), *crateName);
		Destroy();
	}
}