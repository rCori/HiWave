// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "DashingEnemy.generated.h"

class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class HIWAVE_API ADashingEnemy : public AEnemyPawn
{
	GENERATED_BODY()

public:
	ADashingEnemy();

	virtual void EnemyDeath() override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;
	
};
