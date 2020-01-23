// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAI/EnemyAI.h"
#include "CollidingPawnMovementComponent.h"
#include "HiWavePawn.h"
#include "Components/SphereComponent.h"
#include "HiWaveGameState.h"

// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	StaticMeshComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	StaticMeshComponentPtr->BodyInstance.SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	//StaticMeshComponentPtr->SetNotifyRigidBodyCollision(true);
	//StaticMeshComponentPtr->SetStaticMesh(ShipMesh.Object);
	//StaticMeshComponentPtr->OnComponentHit.AddDynamic(this, &AEnemyPawn::OnHit);// set up a notification for when this component hits something
	RootComponent = StaticMeshComponentPtr;

	//StaticMeshComponentPtr->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPawn::OnOverlap);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HitDetection"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPawn::OnOverlap);
	SphereComponent->SetupAttachment(RootComponent);
	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	//AIControllerClass = AEnemyAI::StaticClass();

	//Assign bot behavior by grabbing the BehaviorTree object in content
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'")); 
	//BotBehavior = BTob.Object;

	//Adding movement component
	//OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	//OurMovementComponent->UpdatedComponent = RootComponent;

	//health = 50.0;

	
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
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
		EnemyDeath();
	}
}

void AEnemyPawn::EnemyDeath() {
	Destroy();
	AHiWaveGameState* hiWaveGameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
	if (hiWaveGameState) {
		hiWaveGameState->IncreasePlayerScore(pointsAwarded);
	}
	OnEnemyDeathDelegate.Broadcast(SpawningGroupTag);
}

void AEnemyPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL){
		UE_LOG(LogPlayerDeath, Warning, TEXT("[AEnemyPawn] OnHit"));
		
		playerActor->TakeHit();
	}
}

void AEnemyPawn::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL && OtherComp->ComponentHasTag("ShipMesh")) {
		//UE_LOG(LogPlayerDeath, Warning, TEXT("[AEnemyPawn %s] OnOverlap"), *GetActorLabel());
		/*
		UE_LOG(LogPlayerDeath, Warning, TEXT("[AEnemyPawn] OverlappedComp->GetFName: %s"), *(OtherComp->GetFName().ToString()));
		
		for (FName tagName : OtherComp->ComponentTags) {
			UE_LOG(LogPlayerDeath, Warning, TEXT("[AEnemyPawn] TagName: %s"), *(tagName.ToString()));
		}
		*/
		playerActor->TakeHit();
	}
}

void AEnemyPawn::SetSpawningGroupTag(FString groupTag) {
	this->SpawningGroupTag = groupTag;
}
