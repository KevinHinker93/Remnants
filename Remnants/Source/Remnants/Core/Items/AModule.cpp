// Fill out your copyright notice in the Description page of Project Settings.

#include "AModule.h"

UAModule::UAModule()
{
	//m_root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	////RootComponent = m_root;

	//// Use a sphere as a simple collision representation
	//m_collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//m_collisionComp->InitSphereRadius(5.0f);
	//m_collisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	//m_collisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	//m_collisionComp->CanCharacterStepUpOn = ECB_No;
	//m_collisionComp->SetupAttachment(m_root);

	//m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	//m_mesh->SetupAttachment(RootComponent);

	m_moduleLevelsToDistribute = 1;
	m_hasRecoil = true;
	m_canFire = true;
}

void UAModule::SetLevelsToDistribute(int maxLevels)
{
	m_moduleLevelsToDistribute = maxLevels;

	float totalDistributionVal = 0;
	TArray<int, TInlineAllocator<4>> unNormalizedDistributionVals;
	for (int i = 0; i < 4; i++)
	{
		float randVal = FMath::RandRange(0.15f, 0.9f);
		totalDistributionVal += randVal;
		unNormalizedDistributionVals[i] = randVal;
	}

	m_damageMultiplier.m_distributionFac = unNormalizedDistributionVals[0] / totalDistributionVal;
	m_firingSpeedDiv.m_distributionFac = unNormalizedDistributionVals[1] / totalDistributionVal;
	m_criticalChance.m_distributionFac = unNormalizedDistributionVals[2] / totalDistributionVal;
	m_criticalDamageMult.m_distributionFac = unNormalizedDistributionVals[3] / totalDistributionVal;
}

int UAModule::GetModuleRarity() const
{
	return m_moduleRarity;
}

bool UAModule::CanFire()
{
	return m_canFire;
}

void UAModule::SetCanFire(bool canFire)
{
	m_canFire = canFire;
}

void UAModule::EnableUse()
{
	m_canFire = true;
	m_world->GetTimerManager().ClearTimer(m_cooldownTimerHandle);
}

void UAModule::CalcModuleRarity()
{
	m_moduleRarity = 0;
	if (m_moduleLevelsToDistribute <= 15)
		m_moduleRarity = 1;
	else if (m_moduleLevelsToDistribute <= 35)
		m_moduleRarity = 2;
	else if (m_moduleLevelsToDistribute <= 60)
		m_moduleRarity = 3;
	else if (m_moduleLevelsToDistribute <= 90)
		m_moduleRarity = 4;
	else if (m_moduleLevelsToDistribute <= 125)
		m_moduleRarity = 5;
	else if (m_moduleLevelsToDistribute > 125)
		m_moduleRarity = 6;
}

float UAModule::GetDamageMult()
{
	/*if (m_moduleLevelsToDistribute != m_lastModuleLevelsToDistribute)
	{
		CalcModuleRarity();
		m_lastModuleLevelsToDistribute = m_moduleLevelsToDistribute;
		
		return m_damageMultiplier.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
	}

	return m_damageMultiplier.m_totalVal;*/
	CalcModuleRarity();
	return m_damageMultiplier.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
}

float UAModule::GetFiringSpeedDiv()
{
	/*if (m_moduleLevelsToDistribute != m_lastModuleLevelsToDistribute)
	{
		CalcModuleRarity();
		m_lastModuleLevelsToDistribute = m_moduleLevelsToDistribute;
		return m_firingSpeedDiv.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
	}

	return m_firingSpeedDiv.m_totalVal;*/
	CalcModuleRarity();
	return m_firingSpeedDiv.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
}

float UAModule::GetCriticalChance()
{
	/*if (m_moduleLevelsToDistribute != m_lastModuleLevelsToDistribute)
	{
		CalcModuleRarity();
		m_lastModuleLevelsToDistribute = m_moduleLevelsToDistribute;
		return m_criticalChance.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
	}

	return m_criticalChance.m_totalVal;*/
	CalcModuleRarity();
	return m_criticalChance.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
}

float UAModule::GetCriticalMult()
{
	/*if (m_moduleLevelsToDistribute != m_lastModuleLevelsToDistribute)
	{
		CalcModuleRarity();
		m_lastModuleLevelsToDistribute = m_moduleLevelsToDistribute;
		return m_criticalDamageMult.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
	}

	return m_criticalDamageMult.m_totalVal;*/
	CalcModuleRarity();
	return m_criticalDamageMult.CalcTotalStat(m_moduleRarity, m_moduleLevelsToDistribute);
}

float UAModule::GetTotalFiringSpeed()
{
	return GetFiringSpeedDiv();
}

// Call function to check if module has a recoil or not
// when firing
bool UAModule::GetHasRecoil()
{
	return m_hasRecoil;
}

bool UAModule::IsInUse()
{
	return false;
}

float UAModule::GetOverheatingMult()
{
	return m_overheatMult;
}