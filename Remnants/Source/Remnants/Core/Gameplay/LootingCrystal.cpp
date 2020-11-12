// Fill out your copyright notice in the Description page of Project Settings.

#include "LootingCrystal.h"
#include "../../EventSystemHandlerSingleton.h"

ALootingCrystal::ALootingCrystal()
{
	PrimaryActorTick.bCanEverTick = false;
	m_hasBeenLooted = false;
}

void ALootingCrystal::Kill()
{
	m_criticalSectionHandle.Lock();
	if (IsPendingKill() || m_hasBeenLooted)
		return;
	ReleaseInventory();
	DestroyCrystal();
	m_hasBeenLooted = true;

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	eventHandler->OnLootingCrystalDestroyed.Broadcast();

	m_criticalSectionHandle.Unlock();
}

