// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HiWaveGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplierLevelChanged, const int&, multiplierLevel);

UCLASS()
class HIWAVE_API AHiWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int IncreasePlayerScore(const int &amount);

	UFUNCTION(BlueprintCallable)
	void IncreaseMultiplier(const float &amount);

	UFUNCTION(BlueprintCallable)
	void ResetMultiplier();

	UFUNCTION(BlueprintCallable)
	float GetMultiplier() const;

	UFUNCTION(BlueprintCallable)
	int GetMultiplierIndex() const;

	UPROPERTY(BlueprintReadOnly)
	int playerScore = 0;

	UPROPERTY(BlueprintReadOnly)
	float currentMultiplier = 1.0;

	UPROPERTY(BlueprintReadOnly)
	int multiplierIndex = 0;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnMultiplierLevelChanged OnMultiplierLevelChanged;
};
