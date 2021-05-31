// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AMainMenuGameMode::BeginPlay()
{
	MainMenuBackgroundWidgetInstantiated = CreateWidget<UUserWidget>(GetWorld(), MainMenuBackgroundWidgetClass);

	if (MainMenuBackgroundWidgetInstantiated != nullptr)
	{
		MainMenuBackgroundWidgetInstantiated->AddToViewport();
	}

	MainMenuButtonsWidgetInstantiated = CreateWidget<UUserWidget>(GetWorld(), MainMenuButtonsWidgetClass);

	if (MainMenuButtonsWidgetInstantiated != nullptr)
	{
		MainMenuButtonsWidgetInstantiated->AddToViewport();
	}


	HiScoreWidgetInstantiated = CreateWidget<UUserWidget>(GetWorld(), HiScoreWidgetClass);

	if (HiScoreWidgetInstantiated != nullptr)
	{
		HiScoreWidgetInstantiated->AddToViewport();
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