// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnSystem.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EnemyPawn.h"
#include "EnemyPawnSpawner.h"


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
	DoDummySpawning();
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::BeginPlay"));
}

// Called every frame
void AEnemySpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnSystem::DoSpawn(FString tag) {
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn"));
	bool contains = SpawnerCollection.Contains(tag);
	if (contains) {
		APawn* pawn = SpawnerCollection[tag]->DoEnemyPawnSpawn();
		AEnemyPawn* enemyPawn = Cast<AEnemyPawn>(pawn);
		if (enemyPawn != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::DoSpawn reference to AEnemyPawn came through clean"));
			enemyPawn->OnEnemyDeathDelegate.AddDynamic(this, &AEnemySpawnSystem::EnemyPawnDeathEventCallback);
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
	TimerDelLeft.BindUFunction(this, FName("DoSpawn"), leftFString);
	TimerDelCenter.BindUFunction(this, FName("DoSpawn"), centerFString);
	TimerDelRight.BindUFunction(this, FName("DoSpawn"), rightFString);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle1, TimerDelLeft, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, TimerDelLeft, 4.f, false);


	GetWorld()->GetTimerManager().SetTimer(TimerHandle3, TimerDelCenter, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle4, TimerDelCenter, 4.f, false);


	GetWorld()->GetTimerManager().SetTimer(TimerHandle5, TimerDelRight, 2.f, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle6, TimerDelRight, 4.f, false);

}


void  AEnemySpawnSystem::EnemyPawnDeathEventCallback(FString enemyTag)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnSystem::EnemyPawnDeathEventCallback(%s)"), *enemyTag);
}