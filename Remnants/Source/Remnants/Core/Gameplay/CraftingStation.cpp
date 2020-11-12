// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingStation.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"


// Sets default values
ACraftingStation::ACraftingStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ACraftingStation::InitializeInteractible()
{

}

void ACraftingStation::InteractionLogic(ACharacter* const character)
{
	UE_LOG(LogTemp, Warning, TEXT("Open crafting menu"));
	OpenInventory(character);
}

void ACraftingStation::DeInitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("DeInitial Craftin Station"));

}

// Called when the game starts or when spawned
void ACraftingStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACraftingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

