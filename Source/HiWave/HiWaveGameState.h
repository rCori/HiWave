// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TutorialCountTypes.h"
#include "HiWaveGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplierLevelChanged, const int&, multiplierLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAwardExtraLife, const int&, lifeIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialValueChanged, ETutorialCountTypes, type, const int&, tutorialValue);

UCLASS()
class HIWAVE_API AHiWaveGameState : public AGameStateBase
{
	GENERATED_BODY()
	virtual void BeginPlay() override; //Override beginplay from the base class

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

	UFUNCTION(BlueprintCallable)
	void UpdateTutorialValue(ETutorialCountTypes valueToUpdate, int newValue = -1);

	UFUNCTION(BlueprintCallable)
	void SaveCurrentGame();

	UFUNCTION(BlueprintCallable)
	void LoadTutorialValues();

	UFUNCTION(BlueprintCallable)
	bool IsTutorialFinished();

	/* This is going to return -1 if tutorial values are not loaded with LoadTutorialValues() beforehand */
	UFUNCTION(BlueprintCallable)
	int GetTutorialValue(ETutorialCountTypes tutorialCountType);

	UPROPERTY(BlueprintReadOnly)
	int playerScore = 0;

	UPROPERTY(BlueprintReadOnly)
	float currentMultiplier = 1.0;

	UPROPERTY(BlueprintReadOnly)
	int multiplierIndex = 0;

	UPROPERTY(BlueprintReadOnly)
	int lifeIncreaseIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lives")
	TArray<int> LifePointIncrease;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lives")
	int MaxLives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tutorial", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float burstGoalPercent = 0.5;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnMultiplierLevelChanged OnMultiplierLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnAwardExtraLife OnAwardExtraLife;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnTutorialValueChanged OnTutorialValueChanged;

private:
	class AHiWaveGameMode* gameMode;
	//class UTutorialProgressSaveGame* saveGame;
	int lifePointIncreaseCount;

	TMap<ETutorialCountTypes, int> tutorialSaveValues;
};
