// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "RedEnemy.generated.h"

class UParticleSystemComponent;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class HIWAVE_API ARedEnemy : public AEnemyPawn
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ARedEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	void SetActive_Implementation(bool IsActive) override;

	/** Particle to emit when this enemy dies */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicFrontMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicSideMaterial;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class AHiWavePawn* playerPawn;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float rotationSpeed;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float fireRate;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	FVector gunOffset;

private:
	float yawDifference;
	FRotator directionToRotate;
	bool bFacingPlayer;
	float fireTimer;
	UClass *redEnemyProjectile;
	UWorld* worldRef;

	FRotator lookAtRotate;
	FVector newDirection;
	FRotator FireRotation;
	FVector SpawnLocation;
	class AItemPool* bulletPool;
};
