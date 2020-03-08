// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawns/EnemyPawn.h"
#include "SpinningEnemy.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API ASpinningEnemy : public AEnemyPawn
{
	GENERATED_BODY()
	
	// Sets default values for this pawn's properties
	ASpinningEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UBoxComponent* ArmBoxComponent;

	float arcCurrTime;
	float arcTimer;
	FVector currentMovementDirection;

};
