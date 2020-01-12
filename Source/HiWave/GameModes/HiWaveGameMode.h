// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HiWaveGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyAndRespawnPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyAllEnemies);

class UUserWidget;

UCLASS(MinimalAPI)
class AHiWaveGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override; //Override beginplay from the base class

public:
	AHiWaveGameMode();

	UFUNCTION()
	void DestroyAndRespawnPlayer();

	UFUNCTION()
	void DestroyAllEnemies();

	UFUNCTION(BlueprintCallable)
	void OpenPauseMenu();

	UPROPERTY(BlueprintReadWrite)
	int playerLives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnDestroyAndRespawnPlayer OnDestroyAndRespawnPlayer;

	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnDestroyAllEnemies OnDestroyAllEnemies;
private:
	bool bIsPaused;

	UUserWidget* PauseWidget;
};



