// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"

int AHiWaveGameState::IncreasePlayerScore(int amount)
{
	playerScore += amount;
	return playerScore;
}
