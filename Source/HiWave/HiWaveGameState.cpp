// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"
#include "Engine/World.h"
#include "GameModes/HiWaveGameMode.h"

int AHiWaveGameState::IncreasePlayerScore(const int &amount)
{
	if (!gameMode) {
		gameMode = Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode());
		lifePointIncreaseCount = LifePointIncrease.Num();
	}
	playerScore += FMath::FloorToInt(currentMultiplier) * amount;
	
	if (lifeIncreaseIndex < lifePointIncreaseCount) {
		if (playerScore > LifePointIncrease[lifeIncreaseIndex]) {
			if (gameMode->playerLives < MaxLives) {
				gameMode->IncreasePlayerLives();
				OnAwardExtraLife.Broadcast(lifeIncreaseIndex);
			}
			lifeIncreaseIndex++;
		}
	}
	else {
		if (playerScore > LifePointIncrease[lifePointIncreaseCount - 1] * (lifeIncreaseIndex - lifePointIncreaseCount + 2)) {
			if (gameMode->playerLives < MaxLives) {
				gameMode->IncreasePlayerLives();
				OnAwardExtraLife.Broadcast(lifeIncreaseIndex);
			}
			lifeIncreaseIndex++;
		}
	}
	return playerScore;
}

void AHiWaveGameState::IncreaseMultiplier(const float &amount)
{
	currentMultiplier += amount;
	if (currentMultiplier >= 3.999) {
		currentMultiplier = 3.999;
	}

	if (currentMultiplier < 1.0) {
		currentMultiplier = 1.0;
	}

	if (currentMultiplier - 1 != multiplierIndex) {
		multiplierIndex = currentMultiplier - 1;
		OnMultiplierLevelChanged.Broadcast(multiplierIndex);
	}
}

void AHiWaveGameState::ResetMultiplier()
{
	currentMultiplier = 1.0;
}

float AHiWaveGameState::GetMultiplier() const
{
	return currentMultiplier;
}

int AHiWaveGameState::GetMultiplierIndex() const
{
	return multiplierIndex;
}
