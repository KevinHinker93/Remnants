// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemCollection.h"
#include "ObjectAndNameAsStringProxyArchive.h"
#include "MemoryWriter.h"
#include "MemoryReader.h"
#include "GameFramework/SaveGame.h"
#include "RemnantsLevelSaveGame.generated.h"

class ADoor;
class ALootCrate;
class ASavegameManager;

UCLASS()
class REMNANTS_API URemnantsLevelSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	URemnantsLevelSaveGame();

	FString m_levelName;
	int m_userIdx;

	void SerializeLevel(ASavegameManager* savegameManager);
	void DeserializeLevel(ASavegameManager* savegameManager);

	void SetEnemyLevel(int level) { if (!m_enemyLevelSet) { m_enemyLevel = level; m_enemyLevelSet = true; } }
	int GetEnemyLevel() { return m_enemyLevel; }
	bool EnemyLevelSet() { return m_enemyLevelSet; }
protected:
	// levels
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		bool m_bossDead;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		TArray<bool> m_lockableDoorStates;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		TArray<FString> m_lockableDoorNames;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		TArray<FString> m_destroyedCrateNames;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		TArray<FString> m_collectedOrbNames;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		int m_enemyLevel;
	UPROPERTY(SaveGame, VisibleAnywhere, Category = "Level")
		bool m_enemyLevelSet;
};
