// Fill out your copyright notice in the Description page of Project Settings.

#include "RemnantsSaveGame.h"
#include "Components/WeaponComponent.h"
#include "Core/Characters/ShootergameCharacter.h"
#include "Core/Gameplay/Door.h"
#include "Components/ActorStatsComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/PowerUpComponent.h"
#include "../Items/AItem.h"
#include "../Items/AModule.h"
#include "../Items/AElement.h"

TArray<uint8> /*URemnantsSaveGame::*/SaveObject(UObject* obj)
{
	TArray<uint8> data;
	if (obj == nullptr) return data;

	FMemoryWriter memoryWriter(data, true);
	// use a wrapper archive that converts FNames and UObject*'s to strings that can be read back in
	RemnantsSaveGameArchive archive(memoryWriter);
	// serialize the object
	obj->Serialize(archive);

	return data;
}

UObject* /* URemnantsSaveGame::*/LoadObject(UObject* obj, TArray<uint8> data)
{
	if (data.Num() <= 0) return obj;
	//TODO how to check if valid binary?

	FMemoryReader memoryReader(data, true);
	RemnantsSaveGameArchive archive(memoryReader);
	obj->Serialize(archive);

	return obj;
}

URemnantsSaveGame::URemnantsSaveGame()
{
	m_slotName = TEXT("Player");
	m_userIdx = 0;
}

void URemnantsSaveGame::SerializeItem(UAItem* item)
{
	auto bytes = SaveObject(item);
	m_serializedItemByteSizes.Add(bytes.Num());
	m_serializedItemClassPathNames.Add(item->GetClass()->GetPathName());
	m_serializedItems.Append(bytes);
	m_serializedItemCount++;
	UE_LOG(LogTemp, Display, TEXT("Serialized item with ID %d"), item->GetId());
}

void URemnantsSaveGame::ClearSerializedItems()
{
	m_serializedItems.Empty();
	m_serializedItemByteSizes.Empty();
	m_serializedItemClassPathNames.Empty();
	m_serializedItemCount = 0;
	m_equippedItemBeginIdx = 0;
	m_playerModuleCount = 0;
	m_playerElementSetCount = 0;
}

TArray<UAItem*> URemnantsSaveGame::DeserializeItems(unsigned int startIdx, unsigned int endIdx)
{
	uint8* b = m_serializedItems.GetData();
	// bring pointer to start idx
	if (startIdx > 0)
		for (unsigned int i = 0; i < startIdx; i++)
			b += m_serializedItemByteSizes[i] * sizeof(uint8);

	TArray<UAItem*> target;

	for (unsigned int i = startIdx; i < endIdx; i++)
	{
		UE_LOG(LogTemp, Display, TEXT("Attempting to deserialize item"));

		// get item class
		UClass* c = StaticLoadClass(UObject::StaticClass(), nullptr, *m_serializedItemClassPathNames[i], nullptr, LOAD_None, nullptr);
		UObject* obj = NewObject<UObject>(this, c);

		// copy one item into tmp array
		TArray<uint8> tmp = TArray<uint8>();
		tmp.Append(b, m_serializedItemByteSizes[i]);

		LoadObject(obj, tmp);
		auto item = Cast<UAItem>(obj);

		checkf(item, TEXT("Could not load class \"%s\" while attempting to load inventory"), *m_serializedItemClassPathNames[i]);

		target.Add(item);

		if (i < endIdx - 1)
			b += m_serializedItemByteSizes[i] * sizeof(uint8);
	}
	return target;
}

void URemnantsSaveGame::SerializeItems(UItemCollection* items)
{
	for (auto item : items->GetItems())
		SerializeItem(item);
}

void URemnantsSaveGame::SerializePlayer(AShootergameCharacter* player)
{
	ClearSerializedItems();
	SerializeItems(player->m_inventoryComponent->GetItemCollection());
	SerializeWeapon(player->m_weaponComponent);
	SerializePlayerStats(player->m_actorStatsComponent);
	m_playerDeltaXP = player->m_currentExp;
	m_neededExpForLvlUp = player->m_neededExpForLvlUp;
	m_unlockedCraftingRecipeIds = player->m_inventoryComponent->m_craftableItemIds;
	m_currentPowerUpPickUps = Cast<UPowerUpComponent>(player->GetComponentByClass(UPowerUpComponent::StaticClass()))->GetCurrentPowerUpCount();
	UE_LOG(LogTemp, Display, TEXT("Serialized Player"));
}

void URemnantsSaveGame::DeserializePlayer(AShootergameCharacter* player)
{
	TArray<UAItem*> inv = DeserializeItems(0, m_equippedItemBeginIdx);
	auto itemCollection = player->m_inventoryComponent->GetItemCollection();
	itemCollection->Clear();
	for (auto item : inv)
		itemCollection->AddItem(item);
	DeserializeWeapon(player->m_weaponComponent);
	DeserializePlayerStats(player, player->m_actorStatsComponent);
	player->m_currentExp = m_playerDeltaXP;
	player->m_neededExpForLvlUp = m_neededExpForLvlUp;
	player->m_inventoryComponent->m_craftableItemIds = m_unlockedCraftingRecipeIds;
	Cast<UPowerUpComponent>(player->GetComponentByClass(UPowerUpComponent::StaticClass()))->SetCurrentPowerUpCount(m_currentPowerUpPickUps);

	UE_LOG(LogTemp, Display, TEXT("Deserialized Player"));
}

void URemnantsSaveGame::SerializeWeapon(UWeaponComponent* weapon)
{
	m_equippedItemBeginIdx = m_serializedItemCount;
	m_playerModuleCount = weapon->GetModuleCount();
	m_playerElementSetCount = weapon->GetElementSetCount();
	for (unsigned int i = 0; i < m_playerModuleCount; i++)
	{
		SerializeItem(Cast<UAItem>(weapon->GetModule(i)));

		for (unsigned int j = 0; j < m_playerElementSetCount; j++)
			SerializeItem(Cast<UAItem>(weapon->GetElement(j, i)));
	}
}

void URemnantsSaveGame::DeserializeWeapon(UWeaponComponent* weapon)
{
	// todo: weapon set module & element count

	TArray<UAItem*> items = DeserializeItems(m_equippedItemBeginIdx, m_serializedItemCount);

	for (unsigned int i = 0; i < m_playerModuleCount; i++)
	{
		unsigned int moduleIdx = i * (m_playerElementSetCount + 1);
		weapon->SetModule(i, Cast<UAModule>(items[moduleIdx]));

		for (unsigned int j = 0; j < m_playerElementSetCount; j++)
		{
			weapon->SetElement(j, i, Cast<UAElement>(items[moduleIdx + j + 1]));
		}
	}
}

void URemnantsSaveGame::SerializePlayerStats(UActorStatsComponent* stats)
{
	m_playerLevel = stats->GetLevel();
	m_attackDistribution = stats->GetAttackDistributionFac();
	m_healthDistribution = stats->GetHealthDistributionFac();
	m_shieldDistribution = stats->GetShieldDistributionFac();
}

void URemnantsSaveGame::DeserializePlayerStats(AShootergameCharacter* player, UActorStatsComponent* stats)
{
	stats->SetAttackDistributionFac(m_attackDistribution);
	stats->SetHealthDistributionFac(m_healthDistribution);
	stats->SetShieldDistributionFac(m_shieldDistribution);
	player->ChangeLevel(m_playerLevel);
	
}