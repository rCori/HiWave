// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PoolableTypes.generated.h"

/**
 * 
 */
/*
UCLASS()
class HIWAVE_API UPoolableTypes : public UObject
{
	GENERATED_BODY()
	
};
*/


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EPoolableType : uint8
{
	VE_PlayerBullet		UMETA(DisplayName = "PlayerBullet"),
	VE_RedEnemyBullet	UMETA(DisplayName = "RedEnemyBullet"),
	VE_NormalPawn		UMETA(DisplayName = "NormalPawn"),
	VE_RedPawn			UMETA(DisplayName = "RedPawn"),
	VE_DashingPawn		UMETA(DisplayName = "DashingPawn"),
	VE_SpinningPawn		UMETA(DisplayName = "SpinningPawn"),
	VE_CardinalPawn		UMETA(DisplayName = "CardinalPawn"),
	VE_XPawn			UMETA(DisplayName = "XPawn"),
	VE_SkullMinePawn	UMETA(DisplayName = "SkullMinePawn"),
	VE_SkullMineWeapon	UMETA(DisplayName = "SkullMineWeapon"),

};