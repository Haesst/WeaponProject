// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WeaponProjectGameMode.h"
#include "WeaponProjectHUD.h"
#include "WeaponProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWeaponProjectGameMode::AWeaponProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AWeaponProjectHUD::StaticClass();
}
