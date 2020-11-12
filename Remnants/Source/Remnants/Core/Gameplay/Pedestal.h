// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractible.h"
#include "Pedestal.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class REMNANTS_API APedestal : public AAInteractible
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APedestal();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "KeyInserted"))
		void KeyInserted();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PedestalAnimation"))
		void AnimatePedestal();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetPedestalAnimationEntry"))
		void SetPedestalAnimationEntry(bool end);

	// AInteractible functions
	virtual void InteractionLogic(class ACharacter* const character) override;
	virtual void InitializeInteractible() override;
	virtual void DeInitialize() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key items")
		TMap<FName, int> m_requiredKeyItems; // used for parsing

	// Current pedestal slot the player tries to interact with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key items")
		FName m_currentElementSlotName;

	UPROPERTY()
		TMap<int, UStaticMeshComponent*> m_itemMeshes; // used for toggling visibility
	UPROPERTY()
		TMap<int, UBoxComponent*> m_itemInteractionAreas; // used for toggling interaction areas

	// Event listeners
	UFUNCTION()
		void OnCharacterHasItemResponse(AActor* receivingCharacter, bool hasItem);
	UFUNCTION()
		void OnUpdateRequest(TArray<int> equippedItems);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ApplyVisibility();

private:
	TArray<int> m_insertedItemIDs;
	FName m_requestedItemName;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit) override;

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
