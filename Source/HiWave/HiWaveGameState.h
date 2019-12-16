// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HiWaveGameState.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API AHiWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int IncreasePlayerScore(int amount);

	UPROPERTY(BlueprintReadOnly)
	int playerScore = 0;

};
