// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "EnemySpawnPoint.h"
#include "SpawnRowData.h"
#include "EnemySpawnSystem.generated.h"


UCLASS()
class HIWAVE_API AEnemySpawnSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Configuration")
	class UDataTable* SpawningDataTable;

	//This points to the DataTable we are going to base our spawns off of
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Configuration")
	TMap<FString, AEnemySpawnPoint*> SpawnerCollection;

	//Keeps a count of how many enemies are left in any given enemy group
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, int> EnemyGroupCounter;
	
	//This is a collection of what spawning groups are queued up to spawn
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> WaveQueue;

	UPROPERTY(BlueprintReadOnly)
	bool WaveQueueRandomized;

	UFUNCTION(BlueprintCallable)
	void DoSpawn(FString spawnerTag, FString groupTag);

	UFUNCTION(BlueprintCallable)
	void DoDummySpawning();

	/**
	* Read the DataTable SpawningDataTable start spawning enemies.
	*/
	UFUNCTION(BlueprintCallable)
	void SpawnFromDatatable(/*const FString &rowName*/);

	UFUNCTION()
	void EnemyPawnDeathEventCallback(FString enemyTag);
	

private:
	AEnemySpawnPoint* getSpawner(ESpawnPoints spawnPoint);

};
