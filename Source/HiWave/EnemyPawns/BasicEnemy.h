// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "PoolableObjectInterface.h"
#include "BasicEnemy.generated.h"

class UParticleSystemComponent;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class HIWAVE_API ABasicEnemy : public AEnemyPawn
{
	GENERATED_BODY()



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Sets default values for this pawn's properties
	ABasicEnemy();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	virtual void SetActive_Implementation(bool IsActive) override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicMaterial;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class AHiWavePawn* playerPawn;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadonly)
	float UpdateRate;

	/* The rate we accelerate in any direction */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float Acceleration;

	/* Friction to constant apply to speed */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	float Friction;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	FVector CurrentSpeed;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	float MaxSpeed;

private:
	FVector newDirection;
	float updateTimer;
};
