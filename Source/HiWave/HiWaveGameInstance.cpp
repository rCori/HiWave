// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameInstance.h"

bool UHiWaveGameInstance::SubmitHiScore(const int &score)
{
	if (score > playerHiScore) {
		playerHiScore = score;
		return true;
	}
	return false;
}

int UHiWaveGameInstance::GetHiScore() const
{
	return playerHiScore;
}
