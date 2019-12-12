// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawn.h"
#include "RedEnemy.generated.h"

class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class HIWAVE_API ARedEnemy : public AEnemyPawn
{
	GENERATED_BODY()
	
public:
	ARedEnemy();

	virtual void EnemyDeath() override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

};
