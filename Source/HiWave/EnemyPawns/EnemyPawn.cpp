// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CollidingPawnMovementComponent.h"
#include "HiWavePawn.h"
#include "Components/SphereComponent.h"
#include "HiWaveGameState.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "HiWaveGameInstance.h"
#include "HiWaveGameState.h"

// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HitDetection"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPawn::OnOverlap);
	RootComponent = SphereComponent;

	// Create the mesh component
	StaticMeshComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	StaticMeshComponentPtr->BodyInstance.SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	StaticMeshComponentPtr->SetupAttachment(RootComponent);
	//StaticMeshComponentPtr->SetSimulatePhysics(true);
	//StaticMeshComponentPtr->SetNotifyRigidBodyCollision(true);
	//StaticMeshComponentPtr->OnComponentHit.AddDynamic(this, &AEnemyPawn::OnHit);

	Active = false;
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UPawnMovementComponent* AEnemyPawn::GetMovementComponent() const
{
	return OurMovementComponent;
}

void AEnemyPawn::EnemyTakeDamage(float damage) {
	health -= damage * damageRatio;
	if (health <= 0.0) {
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
		EnemyDeath();
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		OnHitEffect();
	}
}

void AEnemyPawn::EnemyDeath() {
	
	AHiWaveGameState* hiWaveGameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
	if (hiWaveGameState) {
		hiWaveGameState->IncreasePlayerScore(pointsAwarded);
		UHiWaveGameInstance *hiWaveGameInstance = Cast<UHiWaveGameInstance>(GetWorld()->GetGameInstance());
		if (hiWaveGameInstance) {
			hiWaveGameInstance->SubmitHiScore(hiWaveGameState->playerScore);
		}
	}
	
	AHiWavePawn* hiWavePawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (hiWavePawn) {
		hiWavePawn->IncreaseBurst(burstAwarded);
	}

	OnEnemyDeathDelegate.Broadcast(SpawningGroupTag);
	//Damage ratio is over 1.0 if it has been hit with burst
	if (damageRatio > 1.0) {
		OnIncreaseMultiplierDelegate.Broadcast(multiplierIncrease);
		//Stop the multiplier from couting down for awhile
		if (hiWavePawn) {
			hiWavePawn->HaltMultiplierDecay();
		}
	}

	//Play camera shake on enemy deaths
	cameraManager->PlayCameraShake(EnemyDeathCameraShake, 1.0f);
	DeactivateEvent();
}

void AEnemyPawn::BurstOverlap() {
	damageRatio = damageRatioOnBurst;
}

void AEnemyPawn::OnHitEffect_Implementation() {
}

void AEnemyPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	UE_LOG(LogTemp, Warning, TEXT("EnemyPawn OnHit"));
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL){
		playerActor->TakeHit();
	}
	else if (OtherComp->ComponentHasTag("StageMesh")) {
		UE_LOG(LogTemp, Warning, TEXT("EnemyPawn has hit the edge of the stage"));
	}
}

void AEnemyPawn::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	UE_LOG(LogTemp, Warning, TEXT("EnemyPawn OnOverlap"));
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL && OtherComp->ComponentHasTag("ShipMesh")) {
		playerActor->TakeHit();
	}
}

void AEnemyPawn::SetSpawningGroupTag(FString groupTag) {
	this->SpawningGroupTag = groupTag;
}


void AEnemyPawn::DeactivateEvent()
{
	//UE_LOG(LogTemp, Warning, TEXT("DeactivateEvent in EnemyPawn please overwrite me"));
	IPoolableObjectInterface::Execute_Deactivate(this);
}

void AEnemyPawn::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &AEnemyPawn::DeactivateEvent, Lifespan, false);
}

void AEnemyPawn::SetActive_Implementation(bool IsActive)
{
	UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation in EnemyPawn please overwrite me"));
}

bool AEnemyPawn::IsActive_Implementation()
{
	UE_LOG(LogTemp,Warning, TEXT("IsActive_Implementation in EnemyPawn will return %s"), Active ? TEXT("true") : TEXT("false"));
	return Active;
}

void AEnemyPawn::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}


