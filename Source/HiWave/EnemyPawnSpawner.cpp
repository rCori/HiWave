// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawnSpawner.h"
#include "EnemyPawn.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "EnemyAI.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "SpawnRowData.h"

// Sets default values
AEnemyPawnSpawner::AEnemyPawnSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(RootComponent);

	BoxComponent->BodyInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

// Called when the game starts or when spawned
void AEnemyPawnSpawner::BeginPlay()
{
	Super::BeginPlay();
	//GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemyPawnSpawner::DoEnemyPawnSpawn, 1.5f, true, -1.0f);
	//DoEnemyPawnSpawn();

}

// Called every frame
void AEnemyPawnSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APawn* AEnemyPawnSpawner::DoEnemyPawnSpawn(EEnemyType enemyType)
{
	FVector extent = BoxComponent->GetScaledBoxExtent();
	FVector origin = GetActorLocation();

	float xLoc = FMath::FRandRange(origin.X - (extent.X/2.0f), origin.X + (extent.X / 2.0f));
	float yLoc = FMath::FRandRange(origin.Y - (extent.Y / 2.0f), origin.Y + (extent.Y / 2.0f));
	float zLoc = FMath::FRandRange(origin.Z - (extent.Z / 2.0f), origin.Z + (extent.Z / 2.0f));

	FVector actorSpawnLocation = FVector(xLoc, yLoc, zLoc);

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;
	ActorSpawnParameters.Instigator = Instigator;
	if (GetWorld()) {
		FRotator rotator = FRotator(0.0, 0.0, 0.0);
		APawn* spawnedActor = GetWorld()->SpawnActor<APawn>(EnemyTypeMap[enemyType],actorSpawnLocation, rotator, ActorSpawnParameters);
		if (spawnedActor != nullptr) {
			spawnedActor->SpawnDefaultController();
			return spawnedActor;
		}
	}
	return nullptr;
}
