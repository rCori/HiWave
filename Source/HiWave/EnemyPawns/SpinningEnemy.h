// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawns/EnemyPawn.h"
#include "SpinningEnemy.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API ASpinningEnemy : public AEnemyPawn/*, public IPoolableObjectInterface*/
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

	//void DeactivateEvent();
	/* Implementation of PoolableObjectInterface */
	//void SetObjectLifeSpan_Implementation(float InLifespan) override;

	void SetActive_Implementation(bool IsActive) override;

	//bool IsActive_Implementation() override;

	//void Deactivate_Implementation() override;

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
	float startingCircleNumber;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float startingRotationSpeed;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float circleNumber;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float rotationSpeed;

	

private:
	float arcCurrTime;
	float arcTimer;
	float circleCounter;
	FVector currentMovementDirection;

	float moveX;
	float moveY;
	FVector newMovementDirection;
	FRotator NewRotation;
	FQuat QuatRotation;
};
