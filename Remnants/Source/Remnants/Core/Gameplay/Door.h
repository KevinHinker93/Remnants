// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../AInteractible.h"
#include "Door.generated.h"

UENUM(BlueprintType)
enum class DoorStates : uint8
{
	Open 	UMETA(DisplayName = "Open"),
	Closed 	UMETA(DisplayName = "Closed"),
};

UCLASS()
class REMNANTS_API ADoor : public AAInteractible
{
	GENERATED_BODY()
	
public:	
	ADoor();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DoorAnimation"))
		void AnimateDoor(int state);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUnlockDoor"))
		void OnUnlockDoor();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DoorUnlocked"))
		void OnDoorUnlocked();

	// AInteractible functions
	virtual void InteractionLogic(ACharacter* const character);
	virtual void InitializeInteractible() override;
	virtual void DeInitialize() override;

	bool IsLocked() { return m_locked; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		int m_requiredItemIdToUnlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		bool m_canBeClosed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		bool m_locked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		bool m_lockable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		bool m_lockAfterEntered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		bool m_closeAfterEntered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		FName m_eventNameToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Texts")
		TArray<FName> m_interactionTexts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Door)
		bool m_canInteract;

	void SetLocked(bool locked);

	// Event listeners
	UFUNCTION()
		void OnPedestalInsertionDelegate(FName eventName);
	UFUNCTION()
		void OnRequestOpenDoorDelegate(FName eventName);
	UFUNCTION()
		void OnStateChangeRequested(const FString& doorName, bool state);
	UFUNCTION()
		void OnCharacterHasItemResponse(AActor* receiver, bool hasItem);

	UFUNCTION(Blueprintcallable, Category = "Door")
		void OpenDoor();
	UFUNCTION(Blueprintcallable, Category = "Door")
		void CloseDoor();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type type) override;
	virtual void Tick(float DeltaTime) override;

private:	
	DoorStates m_currentState;	
	void TryUnlockDoor(class AShootergameCharacter* player);
};
