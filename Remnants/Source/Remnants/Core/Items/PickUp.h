// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShapeComponent.h"
#include "PickUp.generated.h"

class UAItem;

UCLASS()
class REMNANTS_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUp();
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY()
		UAItem* m_item = nullptr;
	/*UPROPERTY()
		UParticleSystemComponent* m_particleComponent;*/
public:	
	virtual void Tick(float DeltaTime) override;
	UAItem* GetItem();
	void SetItem(UAItem* item);
};
