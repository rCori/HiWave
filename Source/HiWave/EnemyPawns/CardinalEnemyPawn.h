// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyPawns/EnemyPawn.h"
#include "HiWavePawn.h"
#include "CardinalEnemyPawn.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ECurrentDirection : uint8
{
	VE_Left		UMETA(DisplayName = "Left"),
	VE_Right	UMETA(DisplayName = "Right"),
	VE_Up		UMETA(DisplayName = "Up"),
	VE_Down		UMETA(DisplayName = "Down"),
};

/**
 * 
 */
UCLASS()
class HIWAVE_API ACardinalEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ACardinalEnemyPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;


	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class AHiWavePawn* playerPawn;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicWingMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicCockpitMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicShipMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicEngineMaterial;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

private:
	float zRotationTarget;
	float rotationDegreesRemaining;
	float timeToMove;
	int movingStatus;
	FRotator directionToRotate;
	ECurrentDirection currentDirection;
	ECurrentDirection nextDirection;

};


