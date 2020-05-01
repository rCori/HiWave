// Fill out your copyright notice in the Description page of Project Settings.


#include "CardinalEnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"

ACardinalEnemyPawn::ACardinalEnemyPawn() : AEnemyPawn() {
	//Create the static mesh for this specific pawn
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	StaticMeshComponentPtr->SetStaticMesh(ShipMesh.Object);

	//Assign bot behavior by grabbing the BehaviorTree object in content
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'"));
	BotBehavior = BTob.Object;

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = 20.0;
	speed = 1250.0;
	pointsAwarded = 50;
	damageRatio = 1.0;
	burstAwarded = 0.5;
}


void ACardinalEnemyPawn::Tick(float DeltaTime){
}


void ACardinalEnemyPawn::BeginPlay() {
	Super::BeginPlay();
}

void ACardinalEnemyPawn::EnemyDeath(){
	Super::EnemyDeath();
}

void ACardinalEnemyPawn::BurstOverlap()
{
}
