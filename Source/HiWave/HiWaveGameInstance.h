// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HiWaveGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API UHiWaveGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	bool SubmitHiScore(int score);

	UFUNCTION(BlueprintCallable)
	int GetHiScore();

private:
	int playerHiScore = 0;

};
