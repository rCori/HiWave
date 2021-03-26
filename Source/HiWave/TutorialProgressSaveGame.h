// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialCountTypes.h"
#include "GameFramework/SaveGame.h"
#include "TutorialProgressSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API UTutorialProgressSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UTutorialProgressSaveGame();

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int redTankMarked;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int redTankDestroyed;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int multiplerTwoX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int multiplerThreeX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int burstRecharge;

};
