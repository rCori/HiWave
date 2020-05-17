// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override; //Override beginplay from the base class

public:

	UFUNCTION(BlueprintCallable)
	void StartGameButton();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainMenu")
	TSubclassOf<UUserWidget> MainMenuBackgroundWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainMenu")
	TSubclassOf<UUserWidget> MainMenuButtonsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MainMenu")
	TSubclassOf<UUserWidget> HiScoreWidgetClass;
};
