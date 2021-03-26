// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TutorialCountTypes.generated.h"

/**
 * 
 */
/*
UCLASS()
class HIWAVE_API UTutorialCountTypes : public UObject
{
	GENERATED_BODY()
	
};
*/

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ETutorialCountTypes : uint8
{
	VE_RedTankMarked		UMETA(DisplayName = "RedTankMarked"),
	VE_RedTankDestroyed		UMETA(DisplayName = "RedTankDestroyed"),
	VE_MultiplierTwoX		UMETA(DisplayName = "MultiplierTwoX"),
	VE_MultiplierThreeX		UMETA(DisplayName = "MultiplierThreeX"),
	VE_BurstRecharge		UMETA(DisplayName = "BurstRecharge"),

};
