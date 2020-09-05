// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawns/EnemyPawn.h"
#include "XEnemyPawn.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API AXEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Sets default values for this pawn's properties
	AXEnemyPawn();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class AHiWavePawn* playerPawn;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float rotationSpeed;

	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	UMaterialInstanceDynamic *dynamicMaterial;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

private:
	FRotator NewRotation;
	FQuat QuatRotation;

};
