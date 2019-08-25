// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "SpawnRowDataChild.generated.h"

/**
 * 

UCLASS()
class HIWAVE_API USpawnRowDataChild : public UObject
{
	GENERATED_BODY()
	

};

 */

USTRUCT(BlueprintType)
struct FSpawnRowDataChild : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 someExtraData;
};