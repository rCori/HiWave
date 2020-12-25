// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"

int AHiWaveGameState::IncreasePlayerScore(const int &amount)
{
	playerScore += FMath::FloorToInt(currentMultiplier) * amount;
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
