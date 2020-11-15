// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"

int AHiWaveGameState::IncreasePlayerScore(int amount)
{
	playerScore += FMath::FloorToInt(currentMultiplier) * amount;
	return playerScore;
}

void AHiWaveGameState::IncreaseMultiplier(float amount)
{
	currentMultiplier += amount;
	if (currentMultiplier >= 3.999) {
		currentMultiplier = 3.999;
	}

	if (currentMultiplier < 1.0) {
		currentMultiplier = 1.0;
	}

	multiplierIndex = currentMultiplier - 1;
}

void AHiWaveGameState::ResetMultiplier()
{
	currentMultiplier = 1.0;
}

float AHiWaveGameState::GetMultiplier()
{
	return currentMultiplier;
}

int AHiWaveGameState::GetMultiplierIndex()
{
	return multiplierIndex;
}
