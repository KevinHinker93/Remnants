#include "RemnantsLevelSaveGame.h"
#include "Core/Gameplay/Door.h"
#include "SavegameManager.h"
#include "Core/Characters/ShootergameCharacter.h"
#include "Components/ActorStatsComponent.h"
#include "EventSystemHandlerSingleton.h"

URemnantsLevelSaveGame::URemnantsLevelSaveGame()
{
	m_levelName = 0;
	m_userIdx = 0;
}

void URemnantsLevelSaveGame::SerializeLevel(ASavegameManager* savegameManager)
{
	m_destroyedCrateNames = savegameManager->m_destroyedCrateNames;
	m_collectedOrbNames = savegameManager->m_collectedOrbNames;
	m_bossDead = savegameManager->m_bossDead;

	m_lockableDoorStates.SetNum(savegameManager->m_lockableDoorValues.Num());
	m_lockableDoorNames.SetNum(savegameManager->m_lockableDoorValues.Num());
	int i = 0;
	for (auto kvp : savegameManager->m_lockableDoorValues)
	{
		m_lockableDoorStates[i] = kvp.Value;
		m_lockableDoorNames[i] = kvp.Key;
		i++;

		UE_LOG(LogTemp, Display, TEXT("Serialized door: %s with state %s"), *kvp.Key, (kvp.Value ? TEXT("locked") : TEXT("unlocked")));
	}
}

void URemnantsLevelSaveGame::DeserializeLevel(ASavegameManager* savegameManager)
{
	savegameManager->m_bossDead = m_bossDead;
	savegameManager->m_collectedOrbNames = m_collectedOrbNames;

	savegameManager->m_destroyedCrateNames = m_destroyedCrateNames;

	for (int i = 0; i < m_lockableDoorStates.Num(); i++)
		savegameManager->SetLockableDoorValue(m_lockableDoorNames[i], m_lockableDoorStates[i]);

	savegameManager->ApplyLockableDoorValues();
	savegameManager->DestroyCrates();
}