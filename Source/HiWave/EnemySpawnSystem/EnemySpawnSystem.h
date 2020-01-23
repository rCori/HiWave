// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "EnemySpawnSystem/EnemySpawnPoint.h"
#include "SpawnRowData.h"
#include "EnemySpawnSystem.generated.h"


//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(LogSpawnSystem, Log, All);

UCLASS()
class HIWAVE_API AEnemySpawnSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnSystem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Configuration")
	class UDataTable* SpawningDataTable;

	/* This points to the DataTable we are going to base our spawns off of */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Configuration")
	TMap<ESpawnPoints, AEnemySpawnPoint*> SpawnerCollection;

	/* Keeps a count of how many enemies are left in any given enemy group */
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, int> EnemyGroupCounter;

	/* Collection of what spawning groups are queued up to spawn */
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> WaveQueue;

	/**
	 * A copy of the WaveQueue before the last spawn
	 * Exists so if the player dies and will respawn it will do so with the same
	 * waves of enemies in the queue
	 */
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> PreviousWaveQueue;

	/**
	 * If true the next wave to spawn will be a random index into WaveQueue and not just the next one
	 */
	UPROPERTY(BlueprintReadOnly)
	bool WaveQueueRandomized;

	/**
	 * Whenever we spawn a wave we save it's name here.
	 * If the player dies we uise this to spawn the last wave over again when
	 * the player respawns
	 */
	UPROPERTY(BlueprintReadWrite)
	FString LastWaveSpawned;

	

	/* Read the DataTable SpawningDataTable start spawning enemies */
	UFUNCTION(BlueprintCallable)
	void SpawnFromDatatable();

	/*  */
	UFUNCTION(BlueprintCallable)
	void SingleSpawnWave(const bool &canShuffleSpawnPoints, const TArray<EEnemyType> &enemies, const TArray<ESpawnPoints> &spawnPoints, const FString &groupName);

	UFUNCTION(BlueprintCallable)
	void SpawnLastWave();

	UFUNCTION()
	void ClearAllSpawnTimers();

	UFUNCTION()
	void EnemyPawnDeathEventCallback(FString enemyTag);

	/**
	 * Used this function previously to test spawning enemies before we were reading from the DataTable
	UFUNCTION(BlueprintCallable)
	void DoDummySpawning();
	*/



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	/* We keep track of all of the spawner timers created so we can stop them all if the player dies */
	TArray<FTimerHandle> spawnTimerCollection;

	/* Simple method for getting a unique group name based on the row name */
	const FString createNewGroupNameForWave(FString rowName) const;

	/* Does the reverse of what createNewGroupNameForWave does */
	const FString getWaveNameFromGroupTag(FString groupName) const;
};