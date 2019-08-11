// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWaveGameMode.h"
#include "HiWavePlayerController.h"
#include "HiWavePawn.h"

AHiWaveGameMode::AHiWaveGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AHiWavePawn::StaticClass();

	// tell your custom game mode to use your custom player controller
	PlayerControllerClass = AHiWavePlayerController::StaticClass();
}

