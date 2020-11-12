// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShootergameGameMode.h"
#include "ShootergameHUD.h"
#include "Core/Characters/ShootergameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShootergameGameMode::AShootergameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Characters/Player/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	//HUDClass = AShootergameHUD::StaticClass();
}
