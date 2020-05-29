// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"

int AHiWaveGameState::IncreasePlayerScore(int amount)
{
	playerScore += amount;
	return playerScore;
}

void AHiWaveGameState::IncreaseMultiplier(float amount)
{
	UE_LOG(LogTemp, Warning, TEXT("changing multiplier by: %f"), amount);
	currentMultiplier += amount;
	if (currentMultiplier >= 4.0) {
		currentMultiplier = 4.0;
	}

	if (currentMultiplier < 1.0) {
		currentMultiplier = 1.0;
	}
}

float AHiWaveGameState::GetMultiplier()
{
	return currentMultiplier;
}
