// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "DashingEnemy.generated.h"

class UParticleSystemComponent;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class HIWAVE_API ADashingEnemy : public AEnemyPawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ADashingEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	void SetActive_Implementation(bool IsActive) override;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class AHiWavePawn* playerPawn;

	UPROPERTY(Category = Gameplay, BlueprintReadWrite, EditDefaultsOnly)
	float maxDashTime;

	UPROPERTY(Category = Gameplay, BlueprintReadWrite, EditDefaultsOnly)
	float turnSpeed;

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

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	float LowSpeedThreshold;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;
	
	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicFrontMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicSideMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicEngineMaterial;

private:
	float yawDifference;
	FRotator directionToRotate;
	FVector dashDirection;
	FVector dashTarget;
	bool bFacingPlayer;
	bool bSlowingDown;
	float dashTimer;

	float dashDistance;
	FVector distanceVec;
	FRotator lookAtRotate;
};
