// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "CharacterTypes.h"
#include "CharacterTextDisplay.generated.h"

/**
 * 
 */
/*
UCLASS()
class HIWAVE_API UCharacterTextDisplay : public UObject
{
	GENERATED_BODY()
	
};
*/

USTRUCT(BlueprintType)
struct FCharacterTextDisplay : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterTypes CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterDescription;
};
