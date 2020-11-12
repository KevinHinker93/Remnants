// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavegameManager.generated.h"

UCLASS()
class REMNANTS_API ASavegameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASavegameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class AAchievementManager* m_achievementManager;

	UFUNCTION()
		void OnKeyHolderDied(int keyItemId);
	/*UFUNCTION()
		void OnLevelChangeStarted(const FString& currentScene, const FString& newScene);*/
	UFUNCTION()
		void OnLevelChangeFinished(const FString& previousScene, const FString& currentScene);
	UFUNCTION()
		void OnCrateDestroyed(FString crateName);
	UFUNCTION()
		void OnSaveRequested();
	UFUNCTION()
		void OnLoadRequested(bool save);
	UFUNCTION()
		void OnLevelSaveRequested();
	UFUNCTION()
		void OnLevelLoadRequested();
	UFUNCTION()
		void OnPlayerSaveRequested();
	UFUNCTION()
		void OnPlayerLoadRequested();
	UFUNCTION()
		void OnDoorStateChanged(const FString& doorName, bool state);
	UFUNCTION()
		void OnBossDied();
	UFUNCTION()
		void OnPedestalUpdated(TArray<int> equippedItems);
	UFUNCTION()
		void OnFirstEnterMainRoom();
	UFUNCTION()
		void OnCombineTutorialCompleted();
	UFUNCTION()
		void OnOrbCollected(const FString& orbName);
	UFUNCTION()
		void OnResetState();
	UFUNCTION()
		void OnBossShieldBroken(const FString shieldName);
	UFUNCTION()
		void OnRequestBossActivation();

	UFUNCTION(BlueprintCallable)
		void Load(bool save);
	UFUNCTION(BlueprintCallable)
		void Save();

	UFUNCTION(BlueprintCallable)
		void Lock() { m_criticalSection.Lock(); }
	UFUNCTION(BlueprintCallable)
		void Unlock() { m_criticalSection.Unlock(); }

	bool m_bossDead;
	FString m_currentLevelName;

	UPROPERTY()
		TMap<FString, bool> m_lockableDoorValues;
	UPROPERTY()
		TArray<FString> m_destroyedCrateNames;
	UPROPERTY()
		TArray<FString> m_collectedOrbNames;
	UPROPERTY()
		TArray<int> m_pedestalItems;
	UPROPERTY()
		TArray<FString> m_visitedMaps;
	bool m_pedestalUpdated = false;

	UPROPERTY()
		TSet<FString> m_brokenBossShields;

	void SetLockableDoorValue(FString name, bool value);
	void ApplyLockableDoorValues();
	void DestroyCrates();

	FTransform m_loadedPlayerTransform;
	bool m_mainLevelEntered = false;
	bool m_crystalCombineTutorialCompleted = false;

	FCriticalSection m_criticalSection;
};
