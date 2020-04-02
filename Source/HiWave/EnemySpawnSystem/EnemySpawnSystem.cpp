// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnSystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SpawnRowData.h"
#include "EnemyPawns/EnemyPawn.h"
#include "EnemySpawnSystem/EnemySpawnPoint.h"
#include "GameModes/HiWaveGameMode.h"
#include <string>

//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(LogSpawnSystem);

// Sets default values
AEnemySpawnSystem::AEnemySpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialSpawnWave = "Wave1";
}

// Called when the game starts or when spawned
void AEnemySpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	WaveQueueRandomized = false;
	//Dummy spawning turned off to test "real" spawning from data table configuration
	//DoDummySpawning();
	if (SpawningDataTable != nullptr) {
		//WaveQueue.Add(FString(TEXT("Wave1")));
		WaveQueue.Add(InitialSpawnWave);
		SpawnFromDatatable();

		Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->OnDestroyAndRespawnPlayer.AddDynamic(this, &AEnemySpawnSystem::SpawnLastWave);
		Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->OnDestroyAllEnemies.AddDynamic(this, &AEnemySpawnSystem::ClearAllSpawnTimers);

	}
	//UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::BeginPlay"));
	spawnTimerCollection = TArray<FTimerHandle>();
}

// Called every frame
void AEnemySpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemySpawnSystem::SpawnFromDatatable()
{
	//If we have nothing to spawn then just return
	if (WaveQueue.Num() == 0) return;
	int indexToGet = 0;
	//If the spawns are supposed to be randomized, pull a random element out of the list
	if (WaveQueueRandomized) {
		int iterCount = WaveQueue.Num();
		for (int i = 0; i < iterCount; i++) {
			indexToGet = FMath::Rand() % WaveQueue.Num();
			FString temp = WaveQueue[i];
			WaveQueue[i] = WaveQueue[indexToGet];
			WaveQueue[indexToGet] = temp;
		}
	}

	PreviousWaveQueue = WaveQueue;

	//Pull the name of the row out and remove it, we are about to spawn and "use it up"
	FString rowName = WaveQueue[0];
	WaveQueue.RemoveAt(0);

	//Save this rowName so we can spawn it agai if the player dies
	LastWaveSpawned = rowName;

	//Get the actual data table row we care about
	FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*rowName), TEXT(""), true);

	//Find the new name
	const FString groupName = createNewGroupNameForWave(rowName);

	UE_LOG(LogTemp, Warning, TEXT("Spawning group with groupName: %s"), *groupName);

	FTimerDelegate singleSpawnDelegate;
	singleSpawnDelegate.BindUFunction(this, FName("SingleSpawnWave"), spawnRowData->canShuffleSpawnPoints, spawnRowData->enemies, spawnRowData->spawnPoints, groupName);

	//If we have an internal spawn count of 1 we can just spawn once right now
	if (spawnRowData->spawnCount == 1) {
		SingleSpawnWave(spawnRowData->canShuffleSpawnPoints, spawnRowData->enemies, spawnRowData->spawnPoints, groupName);
	}
	else {
		//Otherwise we must set up a series of timers to spawn every internal wave at the correct time
		for (int i = 1; i <= spawnRowData->spawnCount; i++) {
			FTimerHandle singleSpawnHandle;
			//Time to spawn current wave will be the timer * spawn index
			//Example: If your timer is 0.5 seconds, first spawn is at 0.0 seconds, second at 0.5, third at 1.0, fourth at 1.5 and so on
			GetWorld()->GetTimerManager().SetTimer(singleSpawnHandle, singleSpawnDelegate, spawnRowData->nextInternalSpawnTimer * i, false);
			//Store the FTimerHandle so we can stop and clear it if the player dies in ClearAllSpawnTimers()
			spawnTimerCollection.Add(singleSpawnHandle);
		}
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

	/* Any spawn row can decide to:
	 * Randomize order of future spawn waves
	 * Force the spawn waves to happen in order
	 * Not change the status of the spawn waves being random or not.
	 */
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

	//If the next spawn is coming from a timer, we set that timer now
	if (spawnRowData->nextSpawnTiming == ENextSpawnTiming::VE_Timer) {
		FTimerDelegate SpawnTimerDelegate;
		FTimerHandle SpawnTimerHandle;
		SpawnTimerDelegate.BindUFunction(this, FName("SpawnFromDatatable"));
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, SpawnTimerDelegate, spawnRowData->nextSpawnRowTimer, false);
		//Store the FTimerHandle so we can stop and clear it if the player dies in ClearAllSpawnTimers()
		spawnTimerCollection.Add(SpawnTimerHandle);
	}
}

void AEnemySpawnSystem::SingleSpawnWave(const bool &canShuffleSpawnPoints, const TArray<EEnemyType> &enemies, const TArray<ESpawnPoints> &spawnPoints, const FString &groupName)
{
	int32 enemyCount = 0;
	for (EEnemyType enemy : enemies) {

		//default spawn point index in the row data's list of spawn points is the same as the current enemy index in row data's list of enemies
		int32 spawnPointIndex = enemyCount;
		if (canShuffleSpawnPoints) {
			spawnPointIndex = FMath::Rand() % spawnPoints.Num();
		}

		//Actually grab the AEnemySpawnPoint to use with getSpawner
		AEnemySpawnPoint* spawnerToUse = SpawnerCollection[spawnPoints[spawnPointIndex]];

		//AEnemySpawnPoint will allow us to spawn the enemy actor
		APawn* pawn = spawnerToUse->DoEnemyPawnSpawn(enemy);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);

		//Make sure the spawn was successful
		if (enemyPawn != nullptr) {

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
		//Number of enemies spawned from this wave has increased by 1
		++enemyCount;
	}
}

void AEnemySpawnSystem::SpawnLastWave()
{
	//Since this is happening after the player died and all enemies were wiped off the board
	//We clear anything that was keeping track of current enemies, they are all gone at this moment.
	EnemyGroupCounter.Empty();
	WaveQueue.Empty();
	WaveQueue.Add(LastWaveSpawned);
	for (FString previousWave : PreviousWaveQueue) {
		WaveQueue.Add(previousWave);
	}
	SpawnFromDatatable();
}

void AEnemySpawnSystem::ClearAllSpawnTimers()
{
	//Iterate over all FTimerHandles that were made to spawn enemies
	for (FTimerHandle timerHandle : spawnTimerCollection) {
		if (timerHandle.IsValid()) {
			//Clear the timer so the spawn will not happen.
			GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		}
	}
	//RemoveAt for the entire range of elements in spawnTimerCollection
	if (spawnTimerCollection.Num() > 0) {
		spawnTimerCollection.RemoveAt(0, (spawnTimerCollection.Num() - 1), true);
	}
}


void  AEnemySpawnSystem::EnemyPawnDeathEventCallback(FString enemyTag)
{
	if (EnemyGroupCounter.Contains(enemyTag)) {
		EnemyGroupCounter[enemyTag]--;
		//UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::EnemyPawnDeathEventCallback(%s). There are now only %d enemies left in that group"), *enemyTag, EnemyGroupCounter[enemyTag]);
		if (EnemyGroupCounter[enemyTag] == 0) {
			const FString waveName = getWaveNameFromGroupTag(enemyTag);
			FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*waveName), TEXT(""), true);
			if (spawnRowData->nextSpawnTiming == ENextSpawnTiming::VE_AfterClear) {
				SpawnFromDatatable();
			}
		}
	}
	else {
		//We need a special kind of log if this happens. Not sure if this will cause issues down the line but looks potentially problematic.
		UE_LOG(LogSpawnSystem, Log, TEXT("EnemyPawnDeathEventCallback called on non-existant tag"));
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

/*
void AEnemySpawnSystem::DoSpawn(FString spawnerTag, FString groupTag) {
	//Check if the requested spawner even exists
	bool contains = SpawnerCollection.Contains(spawnerTag);
	if (contains) {
		APawn* pawn = SpawnerCollection[spawnerTag]->DoEnemyPawnSpawn(EEnemyType::VE_NormalPawn);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);
		if (enemyPawn != nullptr) {
			//UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference to AEnemyPawn was valid"));
			//We have to add the Spawn system's callback to the delegate that is called when an individual enemy dies.
			//Cannot do this before enemy spawn as part of the constructor, it must be added at spawn time.
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);
			//The spawning group tag relates an enemy to the group of enemies in a wave it spawned with.
			//EnemyPawnDeathEventCallback uses this to check when an enemy dies how many of it's group are remaining
			enemyPawn->SetSpawningGroupTag(groupTag);
			//If this is the first enemy in this group, add the group tag to the EnemyGroupCounter which keeps track of how many of each
			//wave there are.
			if (!EnemyGroupCounter.Contains(groupTag)) {
				EnemyGroupCounter.Add(groupTag, 0);
			}
			//Increase the count of enemies that exist in this group
			EnemyGroupCounter[groupTag]++;
		}
	}
}
*/

/*
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
	//UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoDummySpawning"));
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
*/