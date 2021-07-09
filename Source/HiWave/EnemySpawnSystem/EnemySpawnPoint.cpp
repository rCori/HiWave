// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnPoint.h"
#include "EnemyPawns/EnemyPawn.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "SpawnRowData.h"
#include "ItemPool.h"
#include "PlayerPawns/HiWavePawn.h"


// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(RootComponent);

	//BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnPoint::OnOverlapBegin);
	//BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawnPoint::OnOverlapEnd);

	//Setup audio component
	spawnAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSoundComponent"));
	spawnAudioComponent->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	spawnAudioComponent->SetSound(SpawnSound);
}

// Called every frame
void AEnemySpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

APawn* AEnemySpawnPoint::DoEnemyPawnSpawn(const EEnemyType &enemyType, const int& recursiveCount)
{
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors, OverlappingCharacter);
	for (AActor* currentActor : overlappingActors) {
		if (!currentActor) {
			UE_LOG(LogTemp, Warning, TEXT("We died at the wrong time and now the game is going to crash"));
		}
		if (recursiveCount > 1000) {
			UE_LOG(LogTemp, Warning, TEXT("The recursiveCount got way too high and now we are going to crash from a stack overflow"));
		}
		AHiWavePawn *player = Cast<AHiWavePawn>(currentActor);
		if (player != nullptr) {
			FString ObjectName = GetName();
			UE_LOG(LogTemp, Warning, TEXT("[EnemySpawnPoint.DoEnemyPawnSpawn] We are spawning from %s even though we are overlapping. reccursiveCount == %d "), *ObjectName, recursiveCount);
			const int randomIndex = FMath::RandRange(0, (NeighborSpawnPoints.Num() - 1));
			return NeighborSpawnPoints[randomIndex]->DoEnemyPawnSpawn(enemyType, recursiveCount + 1);
		}
	}

	/*
	//Pick another spawn point
	if (bIsIntersecting) {
		FString ObjectName = GetName();
		UE_LOG(LogTemp, Warning, TEXT("[EnemySpawnPoint.DoEnemyPawnSpawn] Defering to another spawner Called from %s"), *ObjectName);
		const int randomIndex = FMath::RandRange(0, (NeighborSpawnPoints.Num()-1));
		return NeighborSpawnPoints[randomIndex]->DoEnemyPawnSpawn(enemyType);
	}
	*/
	
	//From the box extent, select the point in the middle of the box
	//We may want to randomize a bit or ignore the box and use a Vector to place the spawn
	const FVector origin = GetActorLocation();

	const FVector actorSpawnLocation = SpawnLocation + origin;

	//Set spawn parameters for call to GetWorld()->SpawnActor
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;
	ActorSpawnParameters.Instigator = Instigator;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (GetWorld()) { 
		IPoolableObjectInterface *poolableObject = ItemPool->GetPooledObject(SpawnEnemyTypeMap[enemyType]);
		APawn* spawnedActor = Cast<APawn>(poolableObject);
		IPoolableObjectInterface::Execute_SetActive(spawnedActor, true);
		spawnedActor->SetActorLocationAndRotation(actorSpawnLocation, FRotator::ZeroRotator);
		if (spawnedActor != nullptr) {
			//This has our enemy AI actually possess the pawn. Otherwise it will have no AI.
			//It uses the default for that pawn class so it works for all enemy types
			spawnedActor->SpawnDefaultController();
			//Play the particle animation
			if (SpawnParticle != nullptr) {	
				FRotator rotation = FRotator::ZeroRotator;
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticle, origin, rotation);
			}
			//Stop the spawn sound if it is playing
			if (spawnAudioComponent->IsPlaying()) {
				spawnAudioComponent->Stop();
			}
			//Play the spawn sound
			spawnAudioComponent->Play();
			return spawnedActor;
		}
	}
	//If the pointer to UWorld was null then this should return nullptr.
	return nullptr;
}

void AEnemySpawnPoint::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != nullptr && player->ActorHasTag("Player"))
	{
		FString ObjectName = GetName();
		UE_LOG(LogTemp, Warning, TEXT("[EnemySpawnPoint.DoEnemyPawnSpawn] Player started intersecting spawn point %s"),*ObjectName);
		bIsIntersecting = true;
	}
}

void AEnemySpawnPoint::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != nullptr && player->ActorHasTag("Player"))
	{
		FString ObjectName = GetName();
		UE_LOG(LogTemp, Warning, TEXT("[EnemySpawnPoint.DoEnemyPawnSpawn] Player has left spawn point %s"), *ObjectName);
		bIsIntersecting = false;
	}
}


