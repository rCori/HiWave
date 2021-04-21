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
	PauseWidget = nullptr;
}


AHiWaveGameMode::AHiWaveGameMode()
{
	// This isn't working for some reason
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_DefaultCharacterPawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	// tell your custom game mode to use your custom player controller
	PlayerControllerClass = AHiWavePlayerController::StaticClass();

	playerLives = 3;
	respawnTime = 2.0;
	playerController = nullptr;
	PauseWidget = nullptr;
}

void AHiWaveGameMode::DestroyAndRespawnPlayer()
{
	AHiWavePawn *playerPawn = Cast<AHiWavePawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	--playerLives;
	OnDestroyAndRespawnPlayer.Broadcast();
	
	AHiWaveGameState *gameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
	if (playerLives > 0) {
		bool destroyable = playerPawn->Destroy();
		RespawnPlayer();
	} else {
		UHiWaveGameInstance *gameInstance = Cast<UHiWaveGameInstance>(GetWorld()->GetGameInstance());
		if (gameState != nullptr && gameInstance != nullptr) {
			gameState->SaveCurrentGame();
			gameInstance->SubmitHiScore(gameState->playerScore);
		}

		UUserWidget* PlayerDeathWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerDeathWidgetClass);
		PlayerDeathWidget->AddToViewport();
	}
}

void AHiWaveGameMode::DestroyAllEnemies() {
	TArray<AActor*> allEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), allEnemies);

	OnDestroyAllEnemies.Broadcast();

	for (AActor *enemyActor : allEnemies) {
		AEnemyPawn *castedEnemyPawn = Cast<AEnemyPawn>(enemyActor);
		if (castedEnemyPawn) {
			IPoolableObjectInterface::Execute_Deactivate(castedEnemyPawn);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("AHiWaveGameMode::DestroyAndRespawnPlayer. Cast to destroy an enemy from world has failed"));
		}
	}
}

void AHiWaveGameMode::OpenPauseMenu() {
	if (playerLives != 0) {
		if (PauseWidget == nullptr) {
			PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
			PauseWidget->AddToViewport();
		}
		else {
			PauseWidget->RemoveFromViewport();
			PauseWidget = nullptr;
		}
	}
}

void AHiWaveGameMode::TogglePause() {
	if (playerLives != 0) {
		bIsPaused = !bIsPaused;
		UGameplayStatics::SetGamePaused(GetWorld(), bIsPaused);
	}
}

void AHiWaveGameMode::RestartGame() const {
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
	if (playerController == nullptr) {
		playerController = UGameplayStatics::GetPlayerController(this, 0);
	}
	
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
}

void AHiWaveGameMode::IncreasePlayerLives() {
	++playerLives;
}