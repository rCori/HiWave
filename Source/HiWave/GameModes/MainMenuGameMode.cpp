// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AMainMenuGameMode::BeginPlay()
{
	UUserWidget* MainMenuBackgroundWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuBackgroundWidgetClass);

	if (MainMenuBackgroundWidget != nullptr)
	{
		MainMenuBackgroundWidget->AddToViewport();
	}

	UUserWidget* MainMenuButtonsWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuButtonsWidgetClass);

	if (MainMenuButtonsWidget != nullptr)
	{
		MainMenuButtonsWidget->AddToViewport();
	}


	UUserWidget* HiScoreWidget = CreateWidget<UUserWidget>(GetWorld(), HiScoreWidgetClass);

	if (HiScoreWidget != nullptr)
	{
		HiScoreWidget->AddToViewport();
	}
}

void AMainMenuGameMode::StartGameButton()
{
	UGameplayStatics::OpenLevel(GetWorld(), "GameMap");
}

void AMainMenuGameMode::ClearSaveData()
{
	UGameplayStatics::DeleteGameInSlot("TutorialSlot", 0);
}