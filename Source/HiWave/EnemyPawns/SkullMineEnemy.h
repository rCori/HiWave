// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyPawns/EnemyPawn.h"
#include "SkullMineEnemy.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EDiagonalDirection : uint8
{
	VE_None			UMETA(DisplayName = "None"),
	VE_UpLeft		UMETA(DisplayName = "UpLeft"),
	VE_UpRight		UMETA(DisplayName = "UpRight"),
	VE_DownLeft		UMETA(DisplayName = "DownLeft"),
	VE_DownRight	UMETA(DisplayName = "DownRight"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESkullEnemyState : uint8
{
	VE_Move		UMETA(DisplayName = "Move"),
	VE_Stop		UMETA(DisplayName = "Stop"),
	VE_Bomb		UMETA(DisplayName = "Bomb"),
};

/**
 * 
 */
UCLASS()
class HIWAVE_API ASkullMineEnemy : public AEnemyPawn
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this pawn's properties
	ASkullMineEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemyDeath() override;

	virtual void BurstOverlap() override;

	virtual void SetActive_Implementation(bool IsActive) override;

	UFUNCTION()
	void OnBarrierOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DoBombAttack();

	UPROPERTY(BlueprintReadonly, EditDefaultsOnly, Category = Gameplay)
	float timeToStop;

	UPROPERTY(BlueprintReadonly, EditDefaultsOnly, Category = Gameplay)
	float timeToMove;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class USphereComponent* BarrierCollisionComponent;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	class UMaterialInstanceDynamic *dynamicBaseMaterial;

	UPROPERTY(Category = Visual, BlueprintReadWrite)
	class UMaterialInstanceDynamic *dynamicSocketMaterial;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	class UParticleSystemComponent* spawnedParticle;


private:
	void initializeDirectionalMap();
	TMap<EDiagonalDirection, FVector> directionalMap;
	EDiagonalDirection directionChosen;
	ESkullEnemyState moveState;
	FVector currentMovementDirection;
	float moveTimer;
	class AItemPool* itemPool;
};
