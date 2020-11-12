// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUp.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EventSystemHandlerSingleton.h"

APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FString name = OtherActor->GetName();

	if (OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Display, TEXT("Pickup collided with player"));
		UShapeComponent* collComp = 0;
		UStaticMeshComponent* meshComp = 0;
		m_item->SetOwner(nullptr);

		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

		if (checkSingleton)
		{
			UE_LOG(LogTemp, Display, TEXT("Send pickup notification to player"));
			eventHandler->OnPickup.Broadcast(OtherActor, GetItem());
		}

		Destroy();
	}
}

void APickUp::BeginPlay()
{
	Super::BeginPlay();

	auto collider = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass()));
	collider->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnOverlapBegin);
}

void APickUp::SetItem(UAItem* item)
{
	m_item = item;
}

UAItem* APickUp::GetItem()
{
	return m_item;
}