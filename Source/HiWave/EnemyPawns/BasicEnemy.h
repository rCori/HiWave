// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "BasicEnemy.generated.h"

class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class HIWAVE_API ABasicEnemy : public AEnemyPawn
{
	GENERATED_BODY()

public:

	// Sets default values for this pawn's properties
	ABasicEnemy();
	
	virtual void EnemyDeath() override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

};
