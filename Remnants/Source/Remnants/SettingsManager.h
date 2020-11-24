// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SettingsManager.generated.h"

UCLASS(Config=Game)
class REMNANTS_API ASettingsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASettingsManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
		void Save();
	UFUNCTION(BlueprintCallable)
		void Load();
	UFUNCTION(BlueprintCallable)
		void Apply();

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		int aaQuality;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		int postprocessingQuality;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		int shadowQuality;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		int textureQuality;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		int effectQuality;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		bool invertX;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		bool invertY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_maxSensitivityGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_lowestSensitivity;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		float mouseSensitivityX;
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
		float mouseSensitivityY;
};
