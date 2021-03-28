// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWaveGameState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TutorialProgressSaveGame.h"
#include "TutorialCountTypes.h"
#include "GameModes/HiWaveGameMode.h"

void AHiWaveGameState::BeginPlay()
{
	
}

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

	if (FMath::TruncToInt(currentMultiplier - 1) != multiplierIndex) {
		int tempMultiplierIndex = FMath::TruncToInt(currentMultiplier - 1);
		if (multiplierIndex == 0 && tempMultiplierIndex == 1) {
			UpdateTutorialValue(ETutorialCountTypes::VE_MultiplierTwoX);
		}
		else if (multiplierIndex == 1 && tempMultiplierIndex == 2) {
			UpdateTutorialValue(ETutorialCountTypes::VE_MultiplierThreeX);
		}

		multiplierIndex = FMath::TruncToInt(currentMultiplier - 1);
		UE_LOG(LogTemp, Warning, TEXT("multiplierIndex is changing to %d"),multiplierIndex);
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

void AHiWaveGameState::UpdateTutorialValue(ETutorialCountTypes valueToUpdate, int newValue)
{
	int currNewValue = newValue;
	if (currNewValue > 10) return;
	//if (saveGame != nullptr) {
	if (tutorialSaveValues.Contains(valueToUpdate)) {
		if (newValue == -1) {
			currNewValue = tutorialSaveValues[valueToUpdate] + 1;
		}
		if (currNewValue <= 10) {
			tutorialSaveValues[valueToUpdate] = currNewValue;
			OnTutorialValueChanged.Broadcast(valueToUpdate, currNewValue);
		}
		/*
		switch (valueToUpdate) {
			case ETutorialCountTypes::VE_RedTankMarked:
				if (newValue == -1) {
					currNewValue = saveGame->redTankMarked + 1;
				}
				if (currNewValue <= 10) {
					saveGame->redTankMarked = currNewValue;
				}
				break;
			case ETutorialCountTypes::VE_RedTankDestroyed:
				if (newValue == -1) {
					currNewValue = saveGame->redTankDestroyed + 1;
				}
				if (currNewValue <= 10) {
					saveGame->redTankDestroyed = currNewValue;
				}
				break;
			case ETutorialCountTypes::VE_MultiplierTwoX:
				if (newValue == -1) {
					currNewValue = saveGame->multiplerTwoX + 1;
				}
				if (currNewValue <= 10) {
					saveGame->multiplerTwoX = currNewValue;
				}
				break;
			case ETutorialCountTypes::VE_MultiplierThreeX:
				if (newValue == -1) {
					currNewValue = saveGame->multiplerThreeX + 1;
				}
				if (currNewValue <= 10) {
					saveGame->multiplerThreeX = currNewValue;
				}
				break;
			case ETutorialCountTypes::VE_BurstRecharge:
				if (newValue == -1) {
					currNewValue = saveGame->burstRecharge + 1;
				}
				if (currNewValue <= 10) {
					saveGame->burstRecharge = currNewValue;
				}
				break;
		}
		
		if (currNewValue <= 10) {
			OnTutorialValueChanged.Broadcast(valueToUpdate, currNewValue);
		}
		*/
	}
}

void AHiWaveGameState::SaveCurrentGame()
{
	UTutorialProgressSaveGame* saveGame = Cast<UTutorialProgressSaveGame>(UGameplayStatics::CreateSaveGameObject(UTutorialProgressSaveGame::StaticClass()));

	if(tutorialSaveValues.Contains(ETutorialCountTypes::VE_RedTankMarked)) {
		saveGame->redTankMarked = tutorialSaveValues[ETutorialCountTypes::VE_RedTankMarked];
	} else {
		saveGame->redTankMarked = 0;
	}

	if (tutorialSaveValues.Contains(ETutorialCountTypes::VE_RedTankDestroyed)) {
		saveGame->redTankDestroyed = tutorialSaveValues[ETutorialCountTypes::VE_RedTankDestroyed];
	}
	else {
		saveGame->redTankDestroyed = 0;
	}

	if (tutorialSaveValues.Contains(ETutorialCountTypes::VE_MultiplierTwoX)) {
		saveGame->multiplerTwoX = tutorialSaveValues[ETutorialCountTypes::VE_MultiplierTwoX];
	}
	else {
		saveGame->multiplerTwoX = 0;
	}

	if (tutorialSaveValues.Contains(ETutorialCountTypes::VE_MultiplierThreeX)) {
		saveGame->multiplerThreeX = tutorialSaveValues[ETutorialCountTypes::VE_MultiplierThreeX];
	}
	else {
		saveGame->multiplerThreeX = 0;
	}
	
	if (tutorialSaveValues.Contains(ETutorialCountTypes::VE_BurstRecharge)) {
		saveGame->burstRecharge = tutorialSaveValues[ETutorialCountTypes::VE_BurstRecharge];
	}
	else {
		saveGame->burstRecharge = 0;
	}

	UGameplayStatics::SaveGameToSlot(saveGame, "TutorialSlot", 0);
}


void AHiWaveGameState::LoadTutorialValues()
{
	tutorialSaveValues = TMap<ETutorialCountTypes, int>();
	UTutorialProgressSaveGame* saveGame = Cast<UTutorialProgressSaveGame>(UGameplayStatics::LoadGameFromSlot("TutorialSlot", 0));
	if (!saveGame) {
		tutorialSaveValues.Add(ETutorialCountTypes::VE_RedTankMarked, 0);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_RedTankDestroyed, 0);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_MultiplierTwoX, 0);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_MultiplierThreeX, 0);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_BurstRecharge, 0);
	}
	else {
		tutorialSaveValues.Add(ETutorialCountTypes::VE_RedTankMarked, saveGame->redTankMarked);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_RedTankDestroyed, saveGame->redTankDestroyed);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_MultiplierTwoX, saveGame->multiplerTwoX);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_MultiplierThreeX, saveGame->multiplerThreeX);
		tutorialSaveValues.Add(ETutorialCountTypes::VE_BurstRecharge, saveGame->burstRecharge);
	}

	OnTutorialValueChanged.Broadcast(ETutorialCountTypes::VE_RedTankMarked, tutorialSaveValues[ETutorialCountTypes::VE_RedTankMarked]);
	OnTutorialValueChanged.Broadcast(ETutorialCountTypes::VE_RedTankDestroyed, tutorialSaveValues[ETutorialCountTypes::VE_RedTankDestroyed]);
	OnTutorialValueChanged.Broadcast(ETutorialCountTypes::VE_MultiplierTwoX, tutorialSaveValues[ETutorialCountTypes::VE_MultiplierTwoX]);
	OnTutorialValueChanged.Broadcast(ETutorialCountTypes::VE_MultiplierThreeX, tutorialSaveValues[ETutorialCountTypes::VE_MultiplierThreeX]);
	OnTutorialValueChanged.Broadcast(ETutorialCountTypes::VE_BurstRecharge, tutorialSaveValues[ETutorialCountTypes::VE_BurstRecharge]);
}


bool AHiWaveGameState::IsTutorialFinished()
{
	return tutorialSaveValues[ETutorialCountTypes::VE_RedTankMarked] == 10
		&& tutorialSaveValues[ETutorialCountTypes::VE_RedTankDestroyed] == 10
		&& tutorialSaveValues[ETutorialCountTypes::VE_MultiplierTwoX] == 10
		&& tutorialSaveValues[ETutorialCountTypes::VE_MultiplierThreeX] == 10
		&& tutorialSaveValues[ETutorialCountTypes::VE_BurstRecharge] == 10;
}

int AHiWaveGameState::GetTutorialValue(ETutorialCountTypes tutorialCountType)
{
	if (tutorialSaveValues.Contains(tutorialCountType)) {
		return tutorialSaveValues[tutorialCountType];
	}
	else {
		return -1;
	}
}
