// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWaveGameMode.h"
#include "HiWavePlayerController.h"
#include "HiWavePawn.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "EnemyPawns/EnemyPawn.h"
#include "HiWaveGameState.h"
#include "HiWaveGameInstance.h"

void AHiWaveGameMode::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsDead = false;

	UUserWidget* GameplayWidget = CreateWidget<UUserWidget>(GetWorld(), GameplayWidgetClass);
	if (GameplayWidget != nullptr)
	{
		GameplayWidget->AddToViewport();
	}
	
	GEngine->GameViewport->Viewport->LockMouseToViewport(true);
}


AHiWaveGameMode::AHiWaveGameMode()
{
	// This isn't working for some reason
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_HiWavePawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	// tell your custom game mode to use your custom player controller
	PlayerControllerClass = AHiWavePlayerController::StaticClass();

	playerLives = 3;
	respawnTime = 2.0;
}

void AHiWaveGameMode::DestroyAndRespawnPlayer()
{
	APawn *playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!playerPawn) return;
	bool destroyable = playerPawn->Destroy();
	APlayerController *playerController = UGameplayStatics::GetPlayerController(this, 0);
	//UE_LOG(LogTemp, Warning, TEXT("Player is destroyable %s"), (destroyable ? TEXT("True") : TEXT("False")));

	OnDestroyAndRespawnPlayer.Broadcast();

	--playerLives;

	AHiWaveGameState *gameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
	if (playerLives > 0) {
		RespawnPlayer();
	} else {
		UHiWaveGameInstance *gameInstance = Cast<UHiWaveGameInstance>(GetWorld()->GetGameInstance());
		if (gameState != nullptr && gameInstance != nullptr) {
			gameInstance->SubmitHiScore(gameState->playerScore);
		}

		UGameplayStatics::OpenLevel(GetWorld(), "MainMenuMap");
	}
}

void AHiWaveGameMode::DestroyAllEnemies() {
	TArray<AActor*> allEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), allEnemies);

	OnDestroyAllEnemies.Broadcast();

	for (AActor *enemyActor : allEnemies) {
		AEnemyPawn *castedEnemyPawn = Cast<AEnemyPawn>(enemyActor);
		if (castedEnemyPawn) {
			//castedEnemyPawn->EnemyDeath();
			//castedEnemyPawn->Destroy();
			IPoolableObjectInterface::Execute_Deactivate(castedEnemyPawn);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("AHiWaveGameMode::DestroyAndRespawnPlayer. Cast to destroy an enemy from world has failed"));
		}
	}
}

void AHiWaveGameMode::OpenPauseMenu() {
	if (!bIsPaused) {
		bIsPaused = true;
		PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
		if (PauseWidget != nullptr)
		{
			PauseWidget->AddToViewport();
		}
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
	else {
		bIsPaused = false;
		if (PauseWidget != nullptr)
		{
			PauseWidget->RemoveFromViewport();
		}
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
}


void AHiWaveGameMode::RestartGame() {
	if (bIsDead) {
		UGameplayStatics::OpenLevel(GetWorld(), "GameMap");
	}
}

void AHiWaveGameMode::PlayerDeath(){
	bIsDead = true;

	UUserWidget* PlayerDeathWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerDeathWidgetClass);
	if (PlayerDeathWidget != nullptr)
	{
		PlayerDeathWidget->AddToViewport();
	}
}


void AHiWaveGameMode::RespawnPlayer() {
	APlayerController *playerController = UGameplayStatics::GetPlayerController(this, 0);
	AHiWaveGameState *gameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
	gameState->ResetMultiplier();
	AActor* playerStart = K2_FindPlayerStart(playerController);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = Instigator;
	APawn *spawnedPlayer = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, playerStart->GetActorLocation(), playerStart->GetActorRotation(), SpawnInfo);
	if (spawnedPlayer != nullptr) {
		playerController->Possess(spawnedPlayer);
	}
	AHiWavePawn *hiWavePawn = Cast<AHiWavePawn>(spawnedPlayer);
	/*
	if (hiWavePawn != nullptr) {
		hiWavePawn->SpawnInvincibility();
	}
	*/
}