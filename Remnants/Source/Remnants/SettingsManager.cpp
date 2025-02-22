// Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsManager.h"
#include "GameFramework/GameUserSettings.h"
#include "Core/Characters/ShootergameCharacter.h"
#include "Engine.h"

// Sets default values
ASettingsManager::ASettingsManager()
{
	PrimaryActorTick.bCanEverTick = false;
	m_maxResolutionScaleGain = 90.0f;
	m_lowestResolutionScale = 10.0f;
	m_maxSensitivityGain = 1.75f;
	m_lowestSensitivity = 0.25f;

	resolutionScale = -1;
	aaQuality = -1;
	postprocessingQuality = -1;
	shadowQuality = -1;
	textureQuality = -1;
	effectQuality = -1;
	mouseSensitivityX = -1.0f;
	mouseSensitivityY = -1.0f;
}

// Called when the game starts or when spawned
void ASettingsManager::BeginPlay()
{
	Super::BeginPlay();	

	bool sectionExists = GConfig->DoesSectionExist(TEXT("/Game/FirstPersonCPP/Blueprints/SettingsManager_BP.SettingsManager_BP_C"), GGameIni);
	if (!sectionExists)
	{
		InitConfigValues();
		SaveConfig();
	}

	Load();
	Apply();
}

// Called every frame
void ASettingsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASettingsManager::Save()
{
	SaveConfig();
	UE_LOG(LogTemp, Log, TEXT("Saved config"));
}

void ASettingsManager::Load()
{
	LoadConfig();
	UE_LOG(LogTemp, Log, TEXT("Loaded config"));
}

void ASettingsManager::Apply()
{
	if (GEngine)
	{
		UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
		userSettings->SetAntiAliasingQuality(aaQuality);
		userSettings->SetPostProcessingQuality(postprocessingQuality);
		userSettings->SetShadowQuality(shadowQuality);
		userSettings->SetTextureQuality(textureQuality);
		userSettings->SetVisualEffectQuality(effectQuality);
		int resolutionScaleVal = m_lowestResolutionScale + ((m_maxResolutionScaleGain / 3.0f) *  resolutionScale);
		userSettings->SetResolutionScaleValueEx(resolutionScaleVal);
		userSettings->ApplySettings(false);

		if (GetWorld())
		{
			ACharacter* playerCharacter =	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			if (!playerCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("No player to apply settings"));
			}
			AShootergameCharacter* player = nullptr;
			if (playerCharacter)
				player = Cast<AShootergameCharacter>(playerCharacter);
			if (player)
			{
				player->m_baseTurnMult = m_lowestSensitivity + (mouseSensitivityX * m_maxSensitivityGain);
				player->m_baseLookUpMult = m_lowestSensitivity + (mouseSensitivityY * m_maxSensitivityGain);
				if (invertX)
					player->m_turnDirection = -1.0f;
				else
					player->m_turnDirection = 1.0f;

				if (invertY)
					player->m_lookUpDirection = -1.0f;
				else
					player->m_lookUpDirection = 1.0f;

				if (mouseWheelInvert)
					player->m_mouseWheelDirection = -1.0f;
				else
					player->m_mouseWheelDirection = 1.0f;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Applied settings"));
	}
}

void ASettingsManager::InitConfigValues()
{
	resolutionScale = m_defaultResolutionScale;
	aaQuality = m_default_aaQuality;
	postprocessingQuality = m_defaultPostprocessingQuality;
	shadowQuality = m_defaultShadowQuality;
	textureQuality = m_defaultTextureQuality;
	effectQuality = m_defaultEffectQuality;
	invertX = m_defaultInvertX;
	invertY = m_defaultInvertY;
	mouseWheelInvert = m_defaultMouseWheelInvert;
	mouseSensitivityX = m_defaultMouseSensitivityX;
	mouseSensitivityY = m_defaultMouseSensitivityY; 
}
