// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnRowData.h"
#include "EnemySpawnPoint.generated.h"

class UBoxComponent;
class APawn;
class AEnemyPawn;

UCLASS()
class HIWAVE_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* 
	 * Spawns an enemy pawn in the center of BoxComponent 
	 * @enemyType Enum specifies which enemy type to spawn
	 */
	UFUNCTION(Category = SpawnSystem)
	APawn* DoEnemyPawnSpawn(EEnemyType enemyType);

	// declare overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/* DoEnemyPawnSpawn will spawn an enemy in the center of this box */
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	UBoxComponent* BoxComponent;

	/* We are switching DoEnemySpawnPawn to spawn at this location instead */
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector SpawnLocation;

	/* Is the player intersecting this spawn point's box component */
	UPROPERTY(BlueprintReadOnly)
	bool bIsIntersecting;

	/*
	 * Internal map of an enum of all enemy pawn classes to those pawn classes themselves
	 * Used by DoEnemyPawnSpawn so it knows what type of enemy pawn to spawn based on enemyType param
	 */
	UPROPERTY(EditDefaultsOnly, Category = SpawningCharacter)
	TMap<EEnemyType, TSubclassOf<AEnemyPawn>> EnemyTypeMap;

	/* DoEnemyPawnSpawn will spawn an enemy in the center of this box */
	UPROPERTY(EditAnywhere)
	TArray<AEnemySpawnPoint*> NeighborSpawnPoints;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
