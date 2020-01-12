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

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;
	
	UMaterialInstanceDynamic *dynamicFrontMaterial;
	UMaterialInstanceDynamic *dynamicSideMaterial;
};
