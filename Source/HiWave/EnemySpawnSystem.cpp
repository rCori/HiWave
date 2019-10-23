// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnSystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SpawnRowData.h"
#include "EnemyPawn.h"
#include "EnemySpawnPoint.h"
#include <string>


// Sets default values
AEnemySpawnSystem::AEnemySpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	WaveQueueRandomized = false;
	//Dummy spawning turned off to test "real" spawning from data table configuration
	//DoDummySpawning();
	WaveQueue.Add(FString(TEXT("Wave1")));
	SpawnFromDatatable(/*FString(TEXT("Wave1"))*/);
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::BeginPlay"));
}

// Called every frame
void AEnemySpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnSystem::DoSpawn(FString spawnerTag, FString groupTag) {
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn"));
	bool contains = SpawnerCollection.Contains(spawnerTag);
	if (contains) {
		APawn* pawn = SpawnerCollection[spawnerTag]->DoEnemyPawnSpawn(EEnemyType::VE_NormalPawn);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);
		if (enemyPawn != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference to AEnemyPawn came through clean"));
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);
			enemyPawn->SetSpawningGroupTag(groupTag);
			if (!EnemyGroupCounter.Contains(groupTag)) {
				EnemyGroupCounter.Add(groupTag, 0);
			}
			EnemyGroupCounter[groupTag]++;
		}
		UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn spawning successful"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn spawner does not exist"));
	}
}


void AEnemySpawnSystem::DoDummySpawning() {
	
	FTimerDelegate TimerDelLeft;
	FTimerDelegate TimerDelCenter;
	FTimerDelegate TimerDelRight;
	FTimerHandle TimerHandle1;
	FTimerHandle TimerHandle2;
	FTimerHandle TimerHandle3;
	FTimerHandle TimerHandle4;
	FTimerHandle TimerHandle5;
	FTimerHandle TimerHandle6;

	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoDummySpawning"));
	FString leftFString = "left";
	FString centerFString = "center";
	FString rightFString = "right";

	FString groupTag = "testGroup";

	TimerDelLeft.BindUFunction(this, FName("DoSpawn"), leftFString, groupTag);
	TimerDelCenter.BindUFunction(this, FName("DoSpawn"), centerFString, groupTag);
	TimerDelRight.BindUFunction(this, FName("DoSpawn"), rightFString, groupTag);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle1, TimerDelLeft, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerDelLeft, 4.f, false);


	GetWorld()->GetTimerManager().SetTimer(TimerHandle3, TimerDelCenter, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle4, TimerDelCenter, 4.f, false);


	GetWorld()->GetTimerManager().SetTimer(TimerHandle5, TimerDelRight, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle6, TimerDelRight, 4.f, false);

}


void AEnemySpawnSystem::SpawnFromDatatable(/*const FString &rowName*/)
{
	//If we have nothing to spawn then just return
	if(WaveQueue.Num() == 0) return;
	int indexToGet = 0;
	//If the spawns are supposed to be randomized, pull a random element out of the list
	if (WaveQueueRandomized) {
		indexToGet = FMath::Rand() % WaveQueue.Num();
	}
	//Pull the name of the row out and remove it, we are about to spawn and "use it up"
	FString rowName = WaveQueue[indexToGet];
	WaveQueue.RemoveAt(indexToGet);

	//Get the actual data table row we care about
	FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*rowName), TEXT(""),true);
		
	//Find the new name
	const FString groupName = createNewGroupNameForWave(rowName);

	int32 enemyCount = 0;
	for (EEnemyType enemy : spawnRowData->enemies) {

		//default spawn point index in the row data's list of spawn points is the same as the current enemy index in row data's list of enemies
		int32 spawnPointIndex = enemyCount;
		if (spawnRowData->canShuffleSpawnPoints) {
			spawnPointIndex = FMath::Rand() % spawnRowData->spawnPoints.Num();
		}

		//Actually grab the AEnemySpawnPoint to use with getSpawner
		ESpawnPoints spawnPoint = spawnRowData->spawnPoints[spawnPointIndex];
		AEnemySpawnPoint* spawnerToUse = getSpawner(spawnPoint);

		//AEnemySpawnPoint will allow us to spawn the enemy actor
		APawn* pawn = spawnerToUse->DoEnemyPawnSpawn(enemy);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);

		//Make sure the spawn was successful
		if (enemyPawn != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference to AEnemyPawn came through clean"));
			
			//So far this is the best way I can  find to give enemies a callback for when they are destroyed. They all have the same event
			//Later if we want different enemies to have different events this could be it's own small function
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);

			//Each enemy is given a group. That group contains a count of how many in that group are left
			//We need to have this because we want to know when all enemies from a group are destroyed.
			//If this is the first enemy from this group, we must make that group new in the map. Otherwise
			//just increment the existing cout
			enemyPawn->SetSpawningGroupTag(groupName);
			if (!EnemyGroupCounter.Contains(groupName)) {
				EnemyGroupCounter.Add(groupName, 0);
			}
			EnemyGroupCounter[groupName]++;
		}
		/*
		else {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference was null"));
		}
		*/
		//Number of enemies spawned from this wave has increased by 1
		++enemyCount;
	}

	//Now that the actual spawning is done set up anything this wave changes for future spawns
	switch (spawnRowData->nextSpawnOverwriteStatus) {
	case ENextSpawnOverwriteStatus::VE_Add:
		for (FString waveName : spawnRowData->spawnWaveNames) {
			WaveQueue.Add(waveName);
		}
		break;
	case ENextSpawnOverwriteStatus::VE_AddIfEmpty:
		if (WaveQueue.Num() == 0) {
			for (FString waveName : spawnRowData->spawnWaveNames) {
				WaveQueue.Add(waveName);
			}
		}
	case ENextSpawnOverwriteStatus::VE_Overwrite:
		WaveQueue.Empty();
		for (FString waveName : spawnRowData->spawnWaveNames) {
			WaveQueue.Add(waveName);
		}
		break;
	case ENextSpawnOverwriteStatus::VE_NoOverwrite:
	default:
		break;
	}

	switch (spawnRowData->nextSpawnListRandomize) {
	case ENextSpawnListRandomize::VE_InOrder:
		WaveQueueRandomized = false;
		break;
	case ENextSpawnListRandomize::VE_Randomized:
		WaveQueueRandomized = true;
		break;
	case ENextSpawnListRandomize::VE_KeepSame:
	default:
		break;
	}

	if (spawnRowData->nextSpawnTiming == ENextSpawnTiming::VE_Timer) {
		FTimerDelegate SpawnTimerDelegate;
		FTimerHandle SpawnTimerHandle;
		SpawnTimerDelegate.BindUFunction(this, FName("SpawnFromDatatable"));
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, SpawnTimerDelegate, spawnRowData->spawnTimer, false);
	}
}

void  AEnemySpawnSystem::EnemyPawnDeathEventCallback(FString enemyTag)
{
	EnemyGroupCounter[enemyTag]--;
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::EnemyPawnDeathEventCallback(%s). There are now only %d enemies left in that group"), *enemyTag, EnemyGroupCounter[enemyTag]);
	if (EnemyGroupCounter[enemyTag] == 0) {
		const FString waveName = getWaveNameFromGroupTag(enemyTag);
		FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*waveName), TEXT(""), true);
		if (spawnRowData->nextSpawnTiming == ENextSpawnTiming::VE_AfterClear) {
			SpawnFromDatatable();
		}
	}
}

AEnemySpawnPoint* AEnemySpawnSystem::getSpawner(ESpawnPoints spawnPoint)
{
	switch (spawnPoint) {
	case ESpawnPoints::VE_Left1:
		return SpawnerCollection["left1"];
		break;
	case ESpawnPoints::VE_Left2:
		return SpawnerCollection["left2"];
		break;
	case ESpawnPoints::VE_Left3:
		return SpawnerCollection["left3"];
		break;
	case ESpawnPoints::VE_Right1:
		return SpawnerCollection["right1"];
		break;
	case ESpawnPoints::VE_Right2:
		return SpawnerCollection["right2"];
		break;
	case ESpawnPoints::VE_Right3:
		return SpawnerCollection["right3"];
		break;
	default:
		return nullptr;

	}
}

const FString AEnemySpawnSystem::createNewGroupNameForWave(FString rowName) const
{

	//This is the map
	//EnemyGroupCounter
	//Start at index 0 looking for a valid new group name
	int currGroupIndex = 0;
	bool groupNameFound = false;
	FString currName = rowName;
	currName.Append("_");
	currName.Append(std::to_string(currGroupIndex).c_str());
	while (!groupNameFound) {
		//If the EnemyGroupCounter map does not contain this name yet then we found an untaken name
		if (!EnemyGroupCounter.Contains(currName)) {
			groupNameFound = true;
		}
		//Otherwise keep searching, try the next name in the series
		else {
			currName = rowName;
			currName.Append("_");
			currName.Append(std::to_string(++currGroupIndex).c_str());
		}
	}

	return currName;
}

const FString AEnemySpawnSystem::getWaveNameFromGroupTag(FString groupName) const
{
	int charIndex = 0;
	FString waveName = groupName;
	groupName.FindLastChar('_', charIndex);
	groupName = groupName.Left(charIndex);
	return groupName;
}
