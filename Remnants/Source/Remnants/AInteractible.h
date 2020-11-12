// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractible.generated.h"

UCLASS(Blueprintable, ABSTRACT)
class REMNANTS_API AAInteractible : public AActor
{
	GENERATED_BODY()
	
public:	
	AAInteractible();

	virtual void InteractionLogic(class ACharacter* const character);
	virtual void InitializeInteractible();
	virtual void DeInitialize();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene component")
		class USceneComponent* m_sceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction area")
		class UBoxComponent* m_interactionArea;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
