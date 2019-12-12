// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnPoint.h"
#include "EnemyPawns/EnemyPawn.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
#include "EnemyAI/EnemyAI.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "SpawnRowData.h"
#include "HiWavePawn.h"

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(RootComponent);

	//BoxComponent->BodyInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnPoint::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawnPoint::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APawn* AEnemySpawnPoint::DoEnemyPawnSpawn(EEnemyType enemyType)
{
	//Pick another spawn point
	if (bIsIntersecting) {
		UE_LOG(LogTemp, Warning, TEXT("[EnemySpawnPoint.DoEnemyPawnSpawn] Defering to another spawner"));
		int randomIndex = FMath::RandRange(0, (NeighborSpawnPoints.Num()-1));
		return NeighborSpawnPoints[randomIndex]->DoEnemyPawnSpawn(enemyType);
	}

	//From the box extent, select the point in the middle of the box
	//We may want to randomize a bit or ignore the box and use a Vector to place the spawn
	//FVector extent = BoxComponent->GetScaledBoxExtent();
	FVector origin = GetActorLocation();

	//float xLoc = FMath::FRandRange(origin.X - (extent.X / 2.0f), origin.X + (extent.X / 2.0f));
	//float yLoc = FMath::FRandRange(origin.Y - (extent.Y / 2.0f), origin.Y + (extent.Y / 2.0f));
	//float zLoc = FMath::FRandRange(origin.Z - (extent.Z / 2.0f), origin.Z + (extent.Z / 2.0f));
	//float zLoc = origin.Z;

	//FVector actorSpawnLocation = FVector(xLoc, yLoc, zLoc);
	FVector actorSpawnLocation = SpawnLocation + origin;

	//Set spawn parameters for call to GetWorld()->SpawnActor
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;
	ActorSpawnParameters.Instigator = Instigator;
	//ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (GetWorld()) { 
		//Spawn the enemy type passed at the location we got from the extent
		APawn* spawnedActor = GetWorld()->SpawnActor<APawn>(EnemyTypeMap[enemyType], actorSpawnLocation, FRotator::ZeroRotator, ActorSpawnParameters);
		if (spawnedActor != nullptr) {
			//This has our enemy AI actually possess the pawn. Otherwise it will have no AI.
			//It uses the default for that pawn class so it works for all enemy types
			spawnedActor->SpawnDefaultController();
			return spawnedActor;
		}
	}
	//If the pointer to UWorld was null then this should return nullptr.
	return nullptr;
}

void AEnemySpawnPoint::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player has entered spawner"));
		bIsIntersecting = true;
	}
}

void AEnemySpawnPoint::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player has left spawner"));
		bIsIntersecting = false;
	}
}


