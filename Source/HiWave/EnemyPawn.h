// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "EnemyPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEnemyPawn, Warning, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeathDelegate, FString, groupTag);

class UBehaviorTree;

UCLASS()
class HIWAVE_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* EnemyMeshComponent;

	

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
	void EnemyTakeDamage(float damage);

	UFUNCTION(BlueprintCallable)
	void EnemyDeath();

	/** Function to handle the enemy hitting the player */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Function for spawning system to set enemy group **/
	UFUNCTION()
	void SetSpawningGroupTag(FString groupTag);

	UFUNCTION()
	float GetSpeed() { return speed; }

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree *BotBehavior;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UCollidingPawnMovementComponent* OurMovementComponent;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMeshComponentPtr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SpawningGroupTag;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnEnemyDeathDelegate OnEnemyDeathDelegate;

protected:
	float health;

	float speed;

};
