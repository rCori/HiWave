// Fill out your copyright notice in the Description page of Project Settings.


#include "CardinalEnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

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
	movingStatus = 0;
	rotationDegreesRemaining = 0.0;
	timeToMove = 0.0;
	directionToRotate = FRotator::ZeroRotator;
	currentDirection = ECurrentDirection::VE_Left;
	nextDirection = ECurrentDirection::VE_Left;
}


void ACardinalEnemyPawn::Tick(float DeltaTime){
	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
	}

	FVector newDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	if (movingStatus == 0) {
		float currentYaw = GetActorRotation().Yaw;
		//Get which component difference is greater
		float newDirX = UKismetMathLibrary::Abs(newDirection.X);
		float newDirY = UKismetMathLibrary::Abs(newDirection.Y);
		//ECurrentDirection canditateDirection;
		if (newDirX > newDirY) {
			if (newDirection.X < 0/* && currentDirection != ECurrentDirection::VE_Down*/) {
				switch (currentDirection) {
				case ECurrentDirection::VE_Right:
					rotationDegreesRemaining = 90.0;
					break;
				case ECurrentDirection::VE_Up:
					rotationDegreesRemaining = 180.0;
					break;
				case ECurrentDirection::VE_Left:
					rotationDegreesRemaining = -90.0;
					break;
				default:
					break;
				}

				nextDirection = ECurrentDirection::VE_Down;
				//movingStatus = 1;
			}
			else if(newDirection.X >= 0/* && currentDirection != ECurrentDirection::VE_Up*/) {
				switch (currentDirection) {
				case ECurrentDirection::VE_Right:
					rotationDegreesRemaining = -90.0;
					break;
				case ECurrentDirection::VE_Down:
					rotationDegreesRemaining = 180.0;
					break;
				case ECurrentDirection::VE_Left:
					rotationDegreesRemaining = 90.0;
					break;
				default:
					break;
				}

				nextDirection = ECurrentDirection::VE_Up;
				//movingStatus = 1;
			}
		}
		else {
			if (newDirection.Y < 0/* && currentDirection != ECurrentDirection::VE_Left*/) {
				switch (currentDirection) {
				case ECurrentDirection::VE_Up:
					rotationDegreesRemaining = -90.0;
					break;
				case ECurrentDirection::VE_Right:
					rotationDegreesRemaining = 180.0;
					break;
				case ECurrentDirection::VE_Down:
					rotationDegreesRemaining = 90.0;
					break;
				default:
					break;
				}

				nextDirection = ECurrentDirection::VE_Left;
				//movingStatus = 1;
			}
			else if(newDirection.Y >= 0/* && currentDirection != ECurrentDirection::VE_Right*/) {
				switch (currentDirection) {
				case ECurrentDirection::VE_Up:
					rotationDegreesRemaining = 90.0;
					break;
				case ECurrentDirection::VE_Left:
					rotationDegreesRemaining = 180.0;
					break;
				case ECurrentDirection::VE_Down:
					rotationDegreesRemaining = -90.0;
					break;
				default:
					break;
				}

				nextDirection = ECurrentDirection::VE_Right;
				//movingStatus = 1;
			}
		}
		//You will move if the nextDirection is equal to the current or the minimum time to move has not yet elapsed.
		if (nextDirection == currentDirection || timeToMove < 0.3f) {
			//Now we are moving for awhile
			FVector movementDirection = FVector::ZeroVector;
			switch (currentDirection) {
			case ECurrentDirection::VE_Up:
				movementDirection = FVector(1, 0, 0);
				break;
			case ECurrentDirection::VE_Down:
				movementDirection = FVector(-1, 0, 0);
				break;
			case ECurrentDirection::VE_Left:
				movementDirection = FVector(0, -1, 0);
				break;
			case ECurrentDirection::VE_Right:
				movementDirection = FVector(0, 1, 0);
				break;
			}
			AddMovementInput(movementDirection, 1.0f);
			timeToMove += DeltaTime;
		}
		else {
			timeToMove = 0.0f;
			movingStatus = 1;
		}
	}
	
	//Moving status 1 means we are in the middle of rotation to our zRotationTarget
	else if (movingStatus == 1) {
		float rotationAmount = DeltaTime * 350.0f;
		if (UKismetMathLibrary::Abs(rotationDegreesRemaining) < UKismetMathLibrary::Abs(rotationAmount)) {
			rotationAmount = rotationDegreesRemaining;
			//movingStatus = 2;
			movingStatus = 0;
			currentDirection = nextDirection;
			switch (currentDirection) {
				case ECurrentDirection::VE_Up:
					directionToRotate.Yaw = 90.0f;
					SetActorRotation(directionToRotate);
					break;
				case ECurrentDirection::VE_Down:
					directionToRotate.Yaw = 270.0f;
					SetActorRotation(directionToRotate);
					break;
				case ECurrentDirection::VE_Left:
					directionToRotate.Yaw = 0.0f;
					SetActorRotation(directionToRotate);
					break;
				case ECurrentDirection::VE_Right:
					directionToRotate.Yaw = 180.0f;
					SetActorRotation(directionToRotate);
					break;
			}
			return;
		}
		if (rotationDegreesRemaining < 0) {
			rotationAmount *= -1.0;
		}

		directionToRotate.Yaw = rotationAmount;
		AddActorLocalRotation(directionToRotate);
		rotationDegreesRemaining -= rotationAmount;

	}
	/*
	else if (movingStatus == 2) {
		//Now we are moving for awhile
		FVector movementDirection = FVector::ZeroVector;
		switch (currentDirection) {
		case ECurrentDirection::VE_Up:
			movementDirection = FVector(1, 0, 0);
			break;
		case ECurrentDirection::VE_Down:
			movementDirection = FVector(-1, 0, 0);
			break;
		case ECurrentDirection::VE_Left:
			movementDirection = FVector(0, -1, 0);
			break;
		case ECurrentDirection::VE_Right:
			movementDirection = FVector(0, 1, 0);
			break;
		}
		AddMovementInput(movementDirection, 1.0f);
		timeToMove -= DeltaTime;
		if (timeToMove <= 0.0) {
			movingStatus = 0;
		}
	}
	*/
}


void ACardinalEnemyPawn::BeginPlay() {
	Super::BeginPlay();
}

void ACardinalEnemyPawn::EnemyDeath(){
	Super::EnemyDeath();
}

void ACardinalEnemyPawn::BurstOverlap()
{
	damageRatio = 2.0;
}
