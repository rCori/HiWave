// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PoolableObjectInterface.h"
#include "EnemyPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEnemyPawn, Warning, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathDelegate, FString, groupTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncreaseMultiplierDelegate, const float&, amount);

class UBehaviorTree;

UCLASS()
class HIWAVE_API AEnemyPawn : public APawn, public IPoolableObjectInterface
{
	GENERATED_BODY()


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Sets default values for this pawn's properties
	AEnemyPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION(BlueprintCallable)
	void EnemyTakeDamage(const float &damage);

	UFUNCTION(BlueprintCallable)
	virtual void EnemyDeath();

	UFUNCTION(BlueprintCallable)
	virtual void BurstOverlap();

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Make the enemy's material flash some color for a second on a timeline"))
	void OnHitEffect();

	/** Function to handle the enemy hitting the player */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Function for spawning system to set enemy group **/
	UFUNCTION()
	void SetSpawningGroupTag(FString groupTag);

	UFUNCTION()
	float GetSpeed() const { return speed; }

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UCollidingPawnMovementComponent* OurMovementComponent;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMeshComponentPtr;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class USphereComponent* SphereComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SpawningGroupTag;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnEnemyDeathDelegate OnEnemyDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FOnIncreaseMultiplierDelegate OnIncreaseMultiplierDelegate;

	UPROPERTY(EditDefaultsOnly, Category = Score)
	int pointsAwarded;

	UPROPERTY(EditDefaultsOnly, Category = Burst)
	float burstAwarded;

	/** Sound to play each time enemy is hit */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* HitSound;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* DeathSound;

	virtual void DeactivateEvent();

	/* Implementation of PoolableObjectInterface */
	void SetObjectLifeSpan_Implementation(float InLifespan) override;
	
	virtual void SetActive_Implementation(bool IsActive) override;

	bool IsActive_Implementation() override;

	void Deactivate_Implementation() override;
	

protected:

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	float startingHealth;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	float health;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
	float damageRatio;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	float damageRatioOnBurst;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	float speed;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	float multiplierIncrease;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCameraShake> EnemyDeathCameraShake;

	UPROPERTY(BlueprintReadOnly)
	APlayerCameraManager* cameraManager;

	float Lifespan = 600.0f;
	bool Active;
	FTimerHandle LifespanTimer;

};
