// Fill out your copyright notice in the Description page of Project Settings.

#include "SavegameManager.h"
#include "../../EventSystemHandlerSingleton.h"
#include "RemnantsSaveGame.h"
#include "RemnantsLevelSaveGame.h"
#include "../Characters/ShootergameCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/ActorStatsComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "../../AchievementManager.h"

// Sets default values
ASavegameManager::ASavegameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASavegameManager::BeginPlay()
{
	Super::BeginPlay();
	
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnKeyHolderDied.AddDynamic(this, &ASavegameManager::OnKeyHolderDied);
	//eventHandler->OnSceneChangeStarted.AddDynamic(this, &ASavegameManager::OnLevelChangeStarted);
	eventHandler->OnSceneChangeFinished.AddDynamic(this, &ASavegameManager::OnLevelChangeFinished);
	eventHandler->OnCrateDestroyed.AddDynamic(this, &ASavegameManager::OnCrateDestroyed);
	eventHandler->OnSaveRequested.AddDynamic(this, &ASavegameManager::OnSaveRequested);
	eventHandler->OnLoadRequested.AddDynamic(this, &ASavegameManager::OnLoadRequested);
	eventHandler->OnDoorStateChanged.AddDynamic(this, &ASavegameManager::OnDoorStateChanged);
	eventHandler->OnBossDied.AddDynamic(this, &ASavegameManager::OnBossDied);
	eventHandler->OnPedestalUpdated.AddDynamic(this, &ASavegameManager::OnPedestalUpdated);
	eventHandler->OnPlayerSaveRequested.AddDynamic(this, &ASavegameManager::OnPlayerSaveRequested);
	eventHandler->OnPlayerLoadRequested.AddDynamic(this, &ASavegameManager::OnPlayerLoadRequested);
	eventHandler->OnLevelSaveRequested.AddDynamic(this, &ASavegameManager::OnLevelSaveRequested);
	eventHandler->OnLevelLoadRequested.AddDynamic(this, &ASavegameManager::OnLevelLoadRequested);
	eventHandler->OnFirstEnterMainRoomTutorial.AddDynamic(this, &ASavegameManager::OnFirstEnterMainRoom);
	eventHandler->OnCombineTutorialCompleted.AddDynamic(this, &ASavegameManager::OnCombineTutorialCompleted);
	eventHandler->OnOrbPickedUp.AddDynamic(this, &ASavegameManager::OnOrbCollected);
	eventHandler->OnResetSavegameManagerState.AddDynamic(this, &ASavegameManager::OnResetState);
	eventHandler->OnBossShieldBroken.AddDynamic(this, &ASavegameManager::OnBossShieldBroken);
	eventHandler->OnRequestBossActivation.AddDynamic(this, &ASavegameManager::OnRequestBossActivation);
}

// Called every frame
void ASavegameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASavegameManager::OnKeyHolderDied(int keyItemId)
{
	m_bossDead = true;
}

void ASavegameManager::OnSaveRequested()
{
	Save();
}

void ASavegameManager::OnLoadRequested(bool save)
{
	Load(save);
}

void ASavegameManager::OnLevelSaveRequested()
{
	auto levelSaveGame = Cast<URemnantsLevelSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsLevelSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(m_currentLevelName, levelSaveGame->m_userIdx))
		levelSaveGame = Cast<URemnantsLevelSaveGame>(UGameplayStatics::LoadGameFromSlot(m_currentLevelName, levelSaveGame->m_userIdx));

	levelSaveGame->SerializeLevel(this);

	if (m_pedestalUpdated)
	{
		auto saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsSaveGame::StaticClass()));
		saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::LoadGameFromSlot(saveGame->m_slotName, saveGame->m_userIdx));
		saveGame->SetPedestalItems(m_pedestalItems);
		UGameplayStatics::SaveGameToSlot(saveGame, saveGame->m_slotName, saveGame->m_userIdx);
		UE_LOG(LogTemp, Display, TEXT("Saved pedestal"));
	}

	auto player = Cast<AShootergameCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter());
	int enemyLevel = player->m_actorStatsComponent->GetLevel();
	levelSaveGame->SetEnemyLevel(enemyLevel);
	UE_LOG(LogTemp, Display, TEXT("Saved enemy level: %d"), enemyLevel);

	UGameplayStatics::SaveGameToSlot(levelSaveGame, m_currentLevelName, levelSaveGame->m_userIdx);
	UE_LOG(LogTemp, Display, TEXT("Saved level %s"), *m_currentLevelName);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSaveFinished.Broadcast();
}

void ASavegameManager::OnLevelLoadRequested()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	auto levelSaveGame = Cast<URemnantsLevelSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsLevelSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(m_currentLevelName, levelSaveGame->m_userIdx))
		levelSaveGame = Cast<URemnantsLevelSaveGame>(UGameplayStatics::LoadGameFromSlot(m_currentLevelName, levelSaveGame->m_userIdx));

	auto saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(saveGame->m_slotName, saveGame->m_userIdx))
	{
		saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::LoadGameFromSlot(saveGame->m_slotName, saveGame->m_userIdx));
		UE_LOG(LogTemp, Display, TEXT("Requested loading level %s"), *saveGame->GetCurrentMapName());

		// only load pedestal, don't load rest of player stuff
		m_pedestalItems = saveGame->GetPedestalItems();

		// load enemy level
		//int enemyLevel = levelSaveGame->EnemyLevelSet() ? levelSaveGame->GetEnemyLevel() : saveGame->GetPlayerLevel();
		int enemyLevel = saveGame->GetPlayerLevel();
		eventHandler->OnSpawnpointLevelChanged.Broadcast(enemyLevel);
		UE_LOG(LogTemp, Display, TEXT("Loaded enemy level: %d"), enemyLevel);
	}

	levelSaveGame->DeserializeLevel(this);
	if (m_bossDead)
		eventHandler->OnKeyHolderKill.Broadcast(-1);
	eventHandler->OnPedestalUpdateRequest.Broadcast(m_pedestalItems);
	for (int gemId : saveGame->GetCraftableItemIds())
		eventHandler->OnGemRecipeDeserialized.Broadcast(gemId);
	for (auto orbName : m_collectedOrbNames)
		eventHandler->OnOrbPickedUp.Broadcast(orbName);

	UE_LOG(LogTemp, Display, TEXT("Loaded level %s & pedestal"), *m_currentLevelName);
}

void ASavegameManager::OnPlayerSaveRequested()
{
	auto saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(saveGame->m_slotName, saveGame->m_userIdx))
		saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::LoadGameFromSlot(saveGame->m_slotName, saveGame->m_userIdx));

	auto player = Cast<AShootergameCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter());
	saveGame->SerializePlayer(player);
	saveGame->SetCurrentMapName(m_currentLevelName);
	saveGame->SetPlayerTransform(player->GetActorTransform());
	saveGame->SetTutorialsCompleted(m_mainLevelEntered);
	saveGame->SetCombineTutorialCompleted(m_crystalCombineTutorialCompleted);
	saveGame->SetVisitedMaps(m_visitedMaps);

	saveGame->m_brokenBossShields = m_brokenBossShields;

	if (m_achievementManager)
	{
		saveGame->m_accumulatedTime = m_achievementManager->GetAccumulatedPlayingTime();
		saveGame->m_accumulatedBossTime = m_achievementManager->GetAccumulatedBossTime();
		saveGame->m_playerDied = m_achievementManager->GetPlayerDied();
		saveGame->m_statAdjusted = m_achievementManager->GetStatChanged();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Achievement manager reference not set in SavegameManager"));
	}

	UGameplayStatics::SaveGameToSlot(saveGame, saveGame->m_slotName, saveGame->m_userIdx);

	player->Reset();
	UE_LOG(LogTemp, Display, TEXT("Saved player in map %s"), *m_currentLevelName);

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnSaveFinished.Broadcast();
}

void ASavegameManager::OnPlayerLoadRequested()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	auto saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(saveGame->m_slotName, saveGame->m_userIdx))
	{
		saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::LoadGameFromSlot(saveGame->m_slotName, saveGame->m_userIdx));

		//m_loadedPlayerTransform = saveGame->GetPlayerTransform();
		auto player = Cast<AShootergameCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter());
		saveGame->DeserializePlayer(player);
		FTransform t = saveGame->GetPlayerTransform();
		//t.AddToTranslation(FVector(0, 0, 200));
		player->SetActorTransform(t);
		UE_LOG(LogTemp, Display, TEXT("Moved player to position: %s"), *saveGame->GetPlayerTransform().GetLocation().ToString());
		player->Reset();

		m_visitedMaps = saveGame->GetVisitedMaps();

		m_mainLevelEntered = saveGame->GetTutorialsCompleted();
		if (m_mainLevelEntered)
			eventHandler->OnTutorialStateLoadedFromSaveGame.Broadcast();

		m_crystalCombineTutorialCompleted = saveGame->GetCombineTutorialCompleted();
		player->m_inventoryComponent->SetGemCombineTutorialCompleted(m_crystalCombineTutorialCompleted);

		m_brokenBossShields = saveGame->m_brokenBossShields;

		if (m_achievementManager)
		{
			m_achievementManager->SetAccumulatedPlayingTime(saveGame->m_accumulatedTime);
			m_achievementManager->SetAccumulatedBossTime(saveGame->m_accumulatedBossTime);
			m_achievementManager->SetPlayerDied(saveGame->m_playerDied);
			m_achievementManager->SetStatChanged(saveGame->m_statAdjusted);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Achievement manager reference not set in SavegameManager"));
		}

		UE_LOG(LogTemp, Display, TEXT("Loaded player"));

		//eventHandler->OnSceneChangeFinished.AddDynamic(this, &ASavegameManager::FinishLoadingLevel);
		//eventHandler->OnSceneChangeRequest.Broadcast(saveGame->GetCurrentMapName());
	}
}

void ASavegameManager::OnPedestalUpdated(TArray<int> equippedItems)
{
	m_pedestalItems = equippedItems;
	m_pedestalUpdated = true;
}

void ASavegameManager::OnResetState()
{
	m_destroyedCrateNames.Empty();
	m_lockableDoorValues.Empty();
	m_collectedOrbNames.Empty();
	m_bossDead = false;
}

void ASavegameManager::OnBossShieldBroken(const FString shieldName)
{
	if(!m_brokenBossShields.Contains(shieldName))
	{
		m_brokenBossShields.Add(shieldName);
		OnSaveRequested();
	}
}

void ASavegameManager::OnRequestBossActivation()
{
	if (m_brokenBossShields.Num() > 0)
	{
		bool checkSingleton = false;
		auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
		eventHandler->OnActivateBossWBrokenShields.Broadcast(m_brokenBossShields);
	}
}

void ASavegameManager::OnLevelChangeFinished(const FString& previousScene, const FString& currentScene)
{
	m_currentLevelName = currentScene;
	if (!m_visitedMaps.Contains(m_currentLevelName))
		m_visitedMaps.Add(m_currentLevelName);
	UE_LOG(LogTemp, Display, TEXT("Active scene in scene manager set to %s"), *m_currentLevelName);
}

void ASavegameManager::OnCrateDestroyed(FString crateName)
{
	m_destroyedCrateNames.Add(crateName);
}

void ASavegameManager::OnDoorStateChanged(const FString& doorName, bool locked)
{
	SetLockableDoorValue(doorName, locked);
}

void ASavegameManager::Save()
{
	OnPlayerSaveRequested();
	OnLevelSaveRequested();
}

void ASavegameManager::Load(bool save)
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	auto saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::CreateSaveGameObject(URemnantsSaveGame::StaticClass()));
	saveGame = Cast<URemnantsSaveGame>(UGameplayStatics::LoadGameFromSlot(saveGame->m_slotName, saveGame->m_userIdx));

	if (saveGame)
	{	
		FString currentMapName = saveGame->GetCurrentMapName();
		eventHandler->OnSceneChangeRequest.Broadcast(currentMapName, save);
	}
}

void ASavegameManager::SetLockableDoorValue(FString name, bool value)
{
	if (!m_lockableDoorValues.Contains(name))
		m_lockableDoorValues.Emplace(name, value);
	else
		m_lockableDoorValues[name] = value;

	UE_LOG(LogTemp, Display, TEXT("Set lockable door %s"), *name);
}

void ASavegameManager::ApplyLockableDoorValues()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	for (auto kvp : m_lockableDoorValues)
	{
		eventHandler->OnDoorStateChangeRequested.Broadcast(kvp.Key, kvp.Value);
	}
}

void ASavegameManager::DestroyCrates()
{
	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);

	for (auto name : m_destroyedCrateNames)
	{
		eventHandler->OnCrateDestructionRequest.Broadcast(name);
	}
}

void ASavegameManager::OnFirstEnterMainRoom()
{
	m_mainLevelEntered = true;
}

void ASavegameManager::OnBossDied()
{
	m_bossDead = true;
	m_brokenBossShields.Empty();
}

void ASavegameManager::OnCombineTutorialCompleted()
{
	m_crystalCombineTutorialCompleted = true;
}

void ASavegameManager::OnOrbCollected(const FString& orbName)
{
	if (!m_collectedOrbNames.Contains(orbName))
		m_collectedOrbNames.Add(orbName);
}