// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HiWaveGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyAndRespawnPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyAllEnemies);

class UUserWidget;
class AHiWaveGameState;

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

	UFUNCTION(BlueprintCallable)
	void TogglePause();

	UFUNCTION(BlueprintCallable)
	void RestartGame() const;

	UFUNCTION(BlueprintCallable)
	void PlayerDeath();

	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();

	UPROPERTY(BlueprintReadWrite)
	int playerLives;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float respawnTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PlayerDeathWidgetClass;

	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnDestroyAndRespawnPlayer OnDestroyAndRespawnPlayer;

	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnDestroyAllEnemies OnDestroyAllEnemies;

private:
	bool bIsPaused;
	bool bIsDead;
	UUserWidget* PauseWidget;
	APlayerController* playerController;
};