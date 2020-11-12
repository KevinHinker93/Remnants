// Fill out your copyright notice in the Description page of Project Settings.

#include "AInteractible.h"
#include "Components/BoxComponent.h"
#include "EventSystemHandlerSingleton.h"


// Sets default values
AAInteractible::AAInteractible()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	m_interactionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));

	m_interactionArea->OnComponentBeginOverlap.AddDynamic(this, &AAInteractible::OnOverlapBegin);
	m_interactionArea->OnComponentEndOverlap.AddDynamic(this, &AAInteractible::OnOverlapEnd);

	RootComponent = m_sceneComponent;
	m_interactionArea->SetupAttachment(RootComponent);
}

void AAInteractible::InteractionLogic(class ACharacter* const character)
{

}

void AAInteractible::InitializeInteractible()
{

}

void AAInteractible::DeInitialize()
{

}

// Called when the game starts or when spawned
void AAInteractible::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAInteractible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAInteractible::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnEnterInteractible.Broadcast(this);
	}
}

void AAInteractible::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnExitInteractible.Broadcast(this);
	}
}