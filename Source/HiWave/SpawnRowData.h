// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "SpawnRowDataChild.h"
#include "SpawnRowData.generated.h"

/**
 * 

UCLASS()
class HIWAVE_API USpawnRowData : public UObject
{
	GENERATED_BODY()
	
};

 */

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EEnemyType : uint8
{
	VE_NormalPawn	UMETA(DisplayName = "NormalPawn"),
	VE_RedPawn		UMETA(DisplayName = "RedPawn"),

};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESpawnPoints : uint8
{
	VE_Left1 	UMETA(DisplayName = "Left1"),
	VE_Left2 	UMETA(DisplayName = "Left2"),
	VE_Left3 	UMETA(DisplayName = "Left3"),
	VE_Right1 	UMETA(DisplayName = "Right1"),
	VE_Right2 	UMETA(DisplayName = "Right2"),
	VE_Right3 	UMETA(DisplayName = "Right3"),

};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ENextSpawnTiming : uint8
{
	VE_AfterClear	UMETA(DisplayName = "AfterClear"),
	VE_Timer		UMETA(DisplayName = "Timer"),
};


UENUM(BlueprintType)	//"BlueprintType" is essential to include
enum class ENextSpawnOverwriteStatus : uint8
{
	VE_NoOverwrite	UMETA(DisplayName = "NoOverwrite"),
	VE_AddIfEmpty	UMETA(DisplayName = "ListAddIfEmpty"),
	VE_Add			UMETA(DisplayName = "ListAdd"),
	VE_Overwrite	UMETA(DisplayName = "ListOverwrite"),
};

UENUM(BlueprintType)	//"BlueprintType" is essential to include
enum class ENextSpawnListRandomize : uint8
{
	VE_KeepSame			UMETA(DisplayName = "KeepSame"),
	VE_Randomized		UMETA(DisplayName = "Random"),
	VE_InOrder			UMETA(DisplayName = "InOrder"),
};

USTRUCT(BlueprintType)
struct FSpawnRowData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EEnemyType> enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESpawnPoints> spawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> spawnWaveNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float spawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canShuffleSpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENextSpawnTiming nextSpawnTiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENextSpawnOverwriteStatus nextSpawnOverwriteStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENextSpawnListRandomize nextSpawnListRandomize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int spawnCount;

};
