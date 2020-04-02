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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Sets default values for this pawn's properties
	ASpinningEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	/** Particle to emit when this enemy dies */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UBoxComponent* ArmBoxComponent;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	class UMaterialInstanceDynamic *dynamicBodyMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	class UMaterialInstanceDynamic *dynamicArmMaterial;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float circleNumber;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float rotationSpeed;

	float arcCurrTime;
	float arcTimer;
	float circleCounter;
	FVector currentMovementDirection;

};