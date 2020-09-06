// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
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
	VE_DashingPawn	UMETA(DisplayName = "DashingPawn"),
	VE_SpinningPawn UMETA(DisplayName = "SpinningPawn"),
	VE_CardinalPawn	UMETA(DisplayName = "CardinalPawn"),
	VE_XPawn		UMETA(DisplayName = "XPawn"),

};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESpawnPoints : uint8
{
	VE_00		UMETA(DisplayName = "[0,0]"),
	VE_10		UMETA(DisplayName = "[1,0]"),
	VE_20		UMETA(DisplayName = "[2,0]"),
	VE_30		UMETA(DisplayName = "[3,0]"),
	VE_40		UMETA(DisplayName = "[4,0]"),

	VE_01		UMETA(DisplayName = "[0,1]"),
	VE_11		UMETA(DisplayName = "[1,1]"),
	VE_21		UMETA(DisplayName = "[2,1]"),
	VE_31		UMETA(DisplayName = "[3,1]"),
	VE_41		UMETA(DisplayName = "[4,1]"),

	VE_02		UMETA(DisplayName = "[0,2]"),
	VE_12		UMETA(DisplayName = "[1,2]"),
	VE_22		UMETA(DisplayName = "[2,2]"),
	VE_32		UMETA(DisplayName = "[3,2]"),
	VE_42		UMETA(DisplayName = "[4,2]"),

	VE_03		UMETA(DisplayName = "[0,3]"),
	VE_13		UMETA(DisplayName = "[1,3]"),
	VE_23		UMETA(DisplayName = "[2,3]"),
	VE_33		UMETA(DisplayName = "[3,3]"),
	VE_43		UMETA(DisplayName = "[4,3]"),

	VE_04		UMETA(DisplayName = "[0,4]"),
	VE_14		UMETA(DisplayName = "[1,4]"),
	VE_24		UMETA(DisplayName = "[2,4]"),
	VE_34		UMETA(DisplayName = "[3,4]"),
	VE_44		UMETA(DisplayName = "[4,4]"),

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
	float nextSpawnRowTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float nextInternalSpawnTimer;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool increaseDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool lastWave;
};
