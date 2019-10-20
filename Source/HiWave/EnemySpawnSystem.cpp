// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnSystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SpawnRowData.h"
#include "EnemyPawn.h"
#include "EnemySpawnPoint.h"


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
	if (WaveQueueRandomized) {
		indexToGet = FMath::Rand() % WaveQueue.Num();
	}
	FString rowName = WaveQueue[indexToGet];
	WaveQueue.RemoveAt(indexToGet);

	static const FString ContextString(TEXT("spawnAfterKilled"));
	//The value of the "spawnAfterKilled" element of the current row
	FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*rowName), TEXT(""),true);
	UE_LOG(LogTemp, Warning, TEXT("RowName: %s - spawnAfterKilled: %b"),*rowName, (spawnRowData->nextSpawnTiming == ENextSpawnTiming::VE_AfterClear));
		
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyType"), true);

	int32 enemyCount = 0;
	for (EEnemyType enemy : spawnRowData->enemies) {
		int32 spawnPointIndex = enemyCount;
		if (spawnRowData->canShuffleSpawnPoints) {
			spawnPointIndex = FMath::Rand() % spawnRowData->spawnPoints.Num();
		}

		ESpawnPoints spawnPoint = spawnRowData->spawnPoints[spawnPointIndex];
		AEnemySpawnPoint* spawnerToUse = getSpawner(spawnPoint);
		APawn* pawn = spawnerToUse->DoEnemyPawnSpawn(enemy);
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);
		if (enemyPawn != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference to AEnemyPawn came through clean"));
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);
			enemyPawn->SetSpawningGroupTag(rowName);
			if (!EnemyGroupCounter.Contains(rowName)) {
				EnemyGroupCounter.Add(rowName, 0);
			}
			EnemyGroupCounter[rowName]++;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference was null"));
		}
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
		FSpawnRowData* spawnRowData = SpawningDataTable->FindRow<FSpawnRowData>(FName(*enemyTag), TEXT(""), true);
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