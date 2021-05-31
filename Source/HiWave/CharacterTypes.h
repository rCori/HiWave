// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterTypes.generated.h"

/**
 * 
 */
/*
UCLASS()
class HIWAVE_API UCharacterTypes : public UObject
{
	GENERATED_BODY()
	
};
*/


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ECharacterTypes : uint8
{
	VE_DefaultCharacter	UMETA(DisplayName = "DefaultCharacter"),
	VE_FastCharacter	UMETA(DisplayName = "FastCharacter"),
	VE_TwinCharacter	UMETA(DisplayName = "TwinCharacter"),

};
