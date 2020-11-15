// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnSystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SpawnRowData.h"
#include "EnemyPawns/EnemyPawn.h"
#include "EnemySpawnSystem/EnemySpawnPoint.h"
#include "GameModes/HiWaveGameMode.h"
#include "ItemPool.h"
#include "HiWaveGameState.h"
#include <string>

//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(LogSpawnSystem);

// Sets default values
AEnemySpawnSystem::AEnemySpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialSpawnWave = "Wave1";
	difficultyIncrease = 0;
	spawnTimerDecrease = 0.0;
	spawnCountIncrease = 0;
	spawnCountIncrease = 0;
	currentChapter = 0;
	chapterTransition = false;
}

// Called when the game starts or when spawned
void AEnemySpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	
	WaveQueueRandomized = false;
	ChangeChapters(0);
	if (CurrentSpawningDataTable != nullptr) {
		WaveQueue.Add(InitialSpawnWave);
		//SpawnFromDatatable();
		Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->OnDestroyAndRespawnPlayer.AddDynamic(this, &AEnemySpawnSystem::SpawnLastWave);
		Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->OnDestroyAllEnemies.AddDynamic(this, &AEnemySpawnSystem::ClearAllSpawnTimers);
		
		
	}
	spawnTimerCollection = TArray<FTimerHandle>();
	if (itemPool != nullptr) {
		itemPool->InitialItemSpawnsFinished.AddDynamic(this, &AEnemySpawnSystem::SpawnFromDatatable);
		itemPool->IncreaseReferenceCountToSpawn();
	}
	
}

// Called every frame
void AEnemySpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemySpawnSystem::SpawnFromDatatable()
{
	UE_LOG(LogSpawnSystem, Warning, TEXT("SpawnFromDatatable"));
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

	//If this is the first wave of the next chapter, then clear out all previous enemies
	if (chapterTransition) {
		chapterTransition = false;
		Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAllEnemies();
	}

	FSpawnRowData* lastSpawnRowData = CurrentSpawningDataTable->FindRow<FSpawnRowData>(FName(*LastWaveSpawned), TEXT(""), true);

	//Pull the name of the row out and remove it, we are about to spawn and "use it up"
	FString rowName = WaveQueue[0];
	WaveQueue.RemoveAt(0);

	//Save this rowName so we can spawn it again if the player dies
	LastWaveSpawned = rowName;

	//Get the actual data table row we care about
	//FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*rowName), TEXT(""), true);
	FSpawnRowData* spawnRowData = CurrentSpawningDataTable->FindRow<FSpawnRowData>(FName(*rowName), TEXT(""), true);

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
		for (int i = 1; i <= spawnRowData->spawnCount + spawnCountIncrease; i++) {
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
		float spawnTime = spawnRowData->nextSpawnRowTimer - spawnTimerDecrease;
		if (spawnTime < 0.2) {
			spawnTime = 0.2;
		}
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, SpawnTimerDelegate, spawnTime, false);
		//Store the FTimerHandle so we can stop and clear it if the player dies in ClearAllSpawnTimers()
		spawnTimerCollection.Add(SpawnTimerHandle);
	}

	//If this wave was set to increase the difficulty, we must apply that difficulty increase
	if (spawnRowData->increaseDifficulty) {
		increaseGameDifficulty();
	}
	//If this wave was the last one for this chapter, go to the next chapter
	if (spawnRowData->lastWave) {
		ChangeChapters(currentChapter + 1);
		chapterTransition = true;
	}
	//Broadcast the event that the wave name is changing.
	OnWaveSpawn.Broadcast(rowName);
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

		AHiWaveGameState* hiWaveGameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());

		//AEnemySpawnPoint will allow us to spawn the enemy actor
		APawn* pawn = spawnerToUse->DoEnemyPawnSpawn(enemy);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);

		//Make sure the spawn was successful
		if (enemyPawn != nullptr) {

			//So far this is the best way I can  find to give enemies a callback for when they are destroyed. They all have the same event
			//Later if we want different enemies to have different events this could be it's own small function
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);
			enemyPawn->OnIncreaseMultiplierDelegate.AddDynamic(hiWaveGameState, &AHiWaveGameState::IncreaseMultiplier);

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
			const FSpawnRowData& spawnRowData = getSpawnRowFromGroupTag(enemyTag);
			if(spawnRowData.nextSpawnTiming == ENextSpawnTiming::VE_AfterClear){
				SpawnFromDatatable();
			}
		}
	}
	else {
		//We need a special kind of log if this happens. Not sure if this will cause issues down the line but looks potentially problematic.
		UE_LOG(LogSpawnSystem, Log, TEXT("EnemyPawnDeathEventCallback called on non-existant tag"));
	}
}

void AEnemySpawnSystem::ChangeChapters(int chapterIndex) {
	FString chapterTitle = ChapterOrder[chapterIndex];
	UDataTable* spawningDataTable = SpawnChapters[chapterTitle];
	UE_LOG(LogSpawnSystem, Warning, TEXT("ChangeChapters"));
	UE_LOG(LogSpawnSystem, Warning, TEXT("Chapter Title: %s") , *chapterTitle);
	if (spawningDataTable != nullptr) {
		currentChapter = chapterIndex;
		CurrentSpawningDataTable = spawningDataTable;
		UE_LOG(LogSpawnSystem, Warning, TEXT("CurrentSpawningDataTable is changing"));
		WaveQueue.Empty();
		WaveQueue.Add(InitialSpawnWave);
		//Broadcast that the chapter name is changing
		OnChangeChapterEvent.Broadcast(chapterTitle);
		//Now that we are changing chapters we can also destroy all enemies on screen
		
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
	currName.Append("_");
	currName.Append(std::to_string(currentChapter).c_str());
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
			currName.Append("_");
			currName.Append(std::to_string(currentChapter).c_str());
		}
	}

	return currName;
}

const FSpawnRowData& AEnemySpawnSystem::getSpawnRowFromGroupTag(const FString &groupName) const
{
	int charIndex = 0;
	FString waveName = groupName;
	//The chapter index will be after the last underscore
	waveName.FindLastChar('_', charIndex);
	const FString chapterIndex = waveName.Mid(charIndex,waveName.Len());
	waveName = groupName.Left(charIndex);
	//Now we seperate the wave counter from the wave name by getting the last underscore position again
	waveName.FindLastChar('_', charIndex);
	waveName = groupName.Left(charIndex);
	const int32 chapterIndexInt = FCString::Atoi(*chapterIndex);
	const FString chapterName = ChapterOrder[chapterIndexInt];
	UDataTable* dataTable = SpawnChapters[chapterName];

	FSpawnRowData* spawnRowData = dataTable->FindRow<FSpawnRowData>(FName(*waveName), TEXT(""), true);
	return *spawnRowData;
}

void AEnemySpawnSystem::increaseGameDifficulty()
{
	difficultyIncrease++;
	int randomInt = FMath::Rand() % 5;

	if (randomInt < 4) {
		spawnTimerDecrease += 0.15;
	}
	else {
		spawnCountIncrease++;
	}
}
