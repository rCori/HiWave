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

	UFUNCTION(BlueprintCallable)
	void IncreaseMultiplier(float amount);

	UFUNCTION(BlueprintCallable)
	void ResetMultiplier();

	UFUNCTION(BlueprintCallable)
	float GetMultiplier();

	UFUNCTION(BlueprintCallable)
	int GetMultiplierIndex();

	UPROPERTY(BlueprintReadOnly)
	int playerScore = 0;

	UPROPERTY(BlueprintReadOnly)
	float currentMultiplier = 1.0;

	UPROPERTY(BlueprintReadOnly)
	int multiplierIndex = 0;

};
