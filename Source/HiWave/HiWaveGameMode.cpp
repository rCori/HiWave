// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWaveGameMode.h"
#include "HiWavePlayerController.h"
#include "HiWavePawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void AHiWaveGameMode::BeginPlay()
{

	UUserWidget* LivesWidget = CreateWidget<UUserWidget>(GetWorld(), LivesWidgetClass);
	if (LivesWidget != nullptr)
	{
		LivesWidget->AddToViewport();
	}
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


}

void AHiWaveGameMode::DestroyAndRespawnPlayer()
{
	APawn *playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!playerPawn) return;
	bool destroyable = playerPawn->Destroy();
	APlayerController *playerController = UGameplayStatics::GetPlayerController(this, 0);
	UE_LOG(LogTemp, Warning, TEXT("Player is destroyable %s"), (destroyable ? TEXT("True") : TEXT("False")));

	--playerLives;

	if (playerLives > 0) {

		FVector Location(-520.0f, 10.0f, 330.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = Instigator;
		APawn *spawnedPlayer = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation, SpawnInfo);
		if (spawnedPlayer != nullptr) {
			playerController->Possess(spawnedPlayer);
		}
	} else {
		UGameplayStatics::OpenLevel(GetWorld(), "MainMenuMap");
	}
}
