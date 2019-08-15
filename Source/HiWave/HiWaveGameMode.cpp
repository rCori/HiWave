// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWaveGameMode.h"
#include "HiWavePlayerController.h"
#include "HiWavePawn.h"
#include "UObject/ConstructorHelpers.h"

AHiWaveGameMode::AHiWaveGameMode()
{
	// set default pawn class to our character class
	//DefaultPawnClass = AHiWavePawn::StaticClass();

	// This isn't working for some reason
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_HiWavePawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	

	// tell your custom game mode to use your custom player controller
	PlayerControllerClass = AHiWavePlayerController::StaticClass();
}

