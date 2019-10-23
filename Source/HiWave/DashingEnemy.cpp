// Fill out your copyright notice in the Description page of Project Settings.


#include "DashingEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CollidingPawnMovementComponent.h"
#include "EnemyAI.h"

ADashingEnemy::ADashingEnemy() : AEnemyPawn() {
	//Create the static mesh for this specific pawn
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	//StaticMeshComponentPtr->SetStaticMesh(ShipMesh.Object);

	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	AIControllerClass = AEnemyAI::StaticClass();

	//Assign bot behavior by grabbing the BehaviorTree object in content
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'"));
	BotBehavior = BTob.Object;

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = 10.0;

	speed = 2000.0;
}
