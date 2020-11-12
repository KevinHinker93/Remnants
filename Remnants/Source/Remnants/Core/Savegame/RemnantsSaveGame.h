// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemCollection.h"
#include "ObjectAndNameAsStringProxyArchive.h"
#include "MemoryWriter.h"
#include "MemoryReader.h"
#include "GameFramework/SaveGame.h"
#include "RemnantsSaveGame.generated.h"

class UWeaponComponent;
class UActorStatsComponent;

// SERIALIZATION LOGIC
struct RemnantsSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	RemnantsSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
	{
		ArIsSaveGame = true;
	}
};

static TArray<uint8> SaveObject(UObject* obj);
static UObject* LoadObject(UObject* obj, TArray<uint8> data);

UCLASS()
class REMNANTS_API URemnantsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:	
	URemnantsSaveGame();

	UPROPERTY(BlueprintReadWrite)
	FString m_slotName;
	UPROPERTY(BlueprintReadWrite)
	int m_userIdx;

	void SerializePlayer(class AShootergameCharacter* player);
	void DeserializePlayer(class AShootergameCharacter* player);
	UFUNCTION(BlueprintCallable)
	void SetCurrentMapName(FString& name) { m_currentMapName = name; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetCurrentMapName() { return m_currentMapName; }
	void SetPedestalItems(TArray<int> itemIds) { m_pedestalItemIDs = itemIds; }
	TArray<int> GetPedestalItems() { return m_pedestalItemIDs; }
	void SetPlayerTransform(FTransform pos) { m_playerTransform = pos; }
	FTransform GetPlayerTransform() { return m_playerTransform; }
	int GetPlayerLevel() { return m_playerLevel; }
	void SetTutorialsCompleted(bool completed) { m_tutorialsCompleted = completed; }
	bool GetTutorialsCompleted() { return m_tutorialsCompleted; }
	void SetCombineTutorialCompleted(bool completed) { m_gemCombineTutorialCompleted = completed; }
	bool GetCombineTutorialCompleted() { return m_gemCombineTutorialCompleted; }
	TArray<int> GetCraftableItemIds() { return m_unlockedCraftingRecipeIds; }
	UFUNCTION(BlueprintCallable)
		TArray<FString> GetVisitedMaps() { return m_visitedMaps; }
	void SetVisitedMaps(TArray<FString> maps) { m_visitedMaps = maps; }

	// achievements
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "Achievement")
		bool m_playerDied;
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "Achievement")
		bool m_statAdjusted;
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "Achievement")
		float m_accumulatedTime;
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "Achievement")
		float m_accumulatedBossTime;

	// boss shields
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Boss Shields")
		TSet<FString> m_brokenBossShields;

protected:
	void SerializeItems(UItemCollection* items);
	void SerializeItem(UAItem* item);
	void SerializeWeapon(UWeaponComponent* weapon);
	void SerializePlayerStats(UActorStatsComponent* stats);

	void DeserializeWeapon(UWeaponComponent* weapon);	
	TArray<UAItem*> DeserializeItems(unsigned int startIdx, unsigned int endIdx);
	void DeserializePlayerStats(AShootergameCharacter* player, UActorStatsComponent* stats);
	void ClearSerializedItems();

	// player inventory
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		unsigned int m_serializedItemCount;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		unsigned int m_equippedItemBeginIdx; // the index where the equipment starts
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		TArray<uint8> m_serializedItems;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		TArray<FString> m_serializedItemClassPathNames;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		TArray<unsigned int> m_serializedItemByteSizes;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		unsigned int m_playerModuleCount;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		unsigned int m_playerElementSetCount;

	// player stats
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		int m_playerLevel;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		float m_playerDeltaXP;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		float m_neededExpForLvlUp;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		float m_healthDistribution;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		float m_shieldDistribution;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		float m_attackDistribution;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		TArray<int> m_unlockedCraftingRecipeIds;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		int m_currentPowerUpPickUps;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		bool m_tutorialsCompleted;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		bool m_gemCombineTutorialCompleted;

	// player position
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Player")
		FTransform m_playerTransform;

	// pedestal
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "World")
		TArray<int> m_pedestalItemIDs;

	// current level
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "World")
		FString m_currentMapName;
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadWrite, Category = "World")
		TArray<FString> m_visitedMaps;
};
