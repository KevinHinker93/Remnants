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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
		float m_maxResolutionScaleGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
		float m_lowestResolutionScale;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int resolutionScale;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int aaQuality;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int postprocessingQuality;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int shadowQuality;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int textureQuality;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Graphics")
		int effectQuality;

	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Mouse")
		bool invertX;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Mouse")
		bool invertY;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Mouse")
		bool mouseWheelInvert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
		float m_maxSensitivityGain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
		float m_lowestSensitivity;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Mouse")
		float mouseSensitivityX;
	UPROPERTY(Config, VisibleAnywhere, BlueprintReadWrite, Category = "Mouse")
		float mouseSensitivityY;
};
