// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HiWaveGameMode.generated.h"

UCLASS(MinimalAPI)
class AHiWaveGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override; //Override beginplay from the base class

public:
	AHiWaveGameMode();

};



