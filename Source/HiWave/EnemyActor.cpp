// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"
#include "HiWavePawn.h"

//General Log
DEFINE_LOG_CATEGORY(LogEnemyActor);

// Sets default values
AEnemyActor::AEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	EnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	//EnemyMeshComponent->BodyInstance.SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	//EnemyMeshComponent->SetNotifyRigidBodyCollision(true);
	EnemyMeshComponent->SetStaticMesh(ShipMesh.Object);
	EnemyMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	EnemyMeshComponent->OnComponentHit.AddDynamic(this, &AEnemyActor::OnHit);		// set up a notification for when this component hits something
	RootComponent = EnemyMeshComponent;
}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemyActor::EnemyTakeDamage(float damage) {
	UE_LOG(LogTemp, Warning, TEXT("Enemy is taking damage"));
}

void AEnemyActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL) {
		UE_LOG(LogTemp, Warning, TEXT("You hit the player"));
	}
}
