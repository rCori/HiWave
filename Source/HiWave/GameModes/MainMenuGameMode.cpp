// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AMainMenuGameMode::BeginPlay()
{
	UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);

	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->AddToViewport();
	}
}

void AMainMenuGameMode::StartGameButton()
{
	UGameplayStatics::OpenLevel(GetWorld(), "GameMap");
}
