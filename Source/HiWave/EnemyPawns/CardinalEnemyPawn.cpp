// Fill out your copyright notice in the Description page of Project Settings.


#include "CardinalEnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"

ACardinalEnemyPawn::ACardinalEnemyPawn() : AEnemyPawn() {
	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;


	health = startingHealth;
	speed = 1250.0;
	pointsAwarded = 50;
	damageRatio = 1.0;
	burstAwarded = 0.5;
	movingStatus = 0;
	rotationDegreesRemaining = 0.0;
	timeToMove = 0.0;
	damageRatioOnBurst = 2.0;
	directionToRotate = FRotator::ZeroRotator;
	currentDirection = ECurrentDirection::VE_Left;
	nextDirection = ECurrentDirection::VE_Left;
	currentYaw = 0.0;
}


void ACardinalEnemyPawn::Tick(float DeltaTime){
	if (!Active) return;
	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
	}

	
	if (movingStatus == 0) {
		newDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		currentYaw = GetActorRotation().Yaw;
		//Get which component difference is greater
		newDirX = UKismetMathLibrary::Abs(newDirection.X);
		newDirY = UKismetMathLibrary::Abs(newDirection.Y);
		if (newDirX > newDirY) {
			if (newDirection.X < 0) {
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
			}
			else if(newDirection.X >= 0) {
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
			}
		}
		else {
			if (newDirection.Y < 0) {
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
			}
			else if(newDirection.Y >= 0) {
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
			}
		}
		//You will move if the nextDirection is equal to the current or the minimum time to move has not yet elapsed.
		if (nextDirection == currentDirection || timeToMove < 0.3f) {
			//Now we are moving for awhile
			movementDirection = FVector::ZeroVector;
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
		newRotationAmount = DeltaTime * 350.0f;
		if (UKismetMathLibrary::Abs(rotationDegreesRemaining) < UKismetMathLibrary::Abs(newRotationAmount)) {
			newRotationAmount = rotationDegreesRemaining;
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
			newRotationAmount *= -1.0;
		}

		directionToRotate.Yaw = newRotationAmount;
		AddActorLocalRotation(directionToRotate);
		rotationDegreesRemaining -= newRotationAmount;
	}
}


void ACardinalEnemyPawn::BeginPlay() {

	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto wingMaterial = staticMesh->GetMaterial(0);
	auto cockpitMaterial = staticMesh->GetMaterial(1);
	auto shipMaterial = staticMesh->GetMaterial(2);
	auto engineMaterial = staticMesh->GetMaterial(3);

	dynamicWingMaterial = UMaterialInstanceDynamic::Create(wingMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicWingMaterial);

	dynamicCockpitMaterial = UMaterialInstanceDynamic::Create(cockpitMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicCockpitMaterial);

	dynamicShipMaterial = UMaterialInstanceDynamic::Create(shipMaterial, NULL);
	staticMesh->SetMaterial(2, dynamicShipMaterial);

	dynamicEngineMaterial = UMaterialInstanceDynamic::Create(engineMaterial, NULL);
	staticMesh->SetMaterial(3, dynamicEngineMaterial);

	Super::BeginPlay();
}

void ACardinalEnemyPawn::EnemyDeath(){
	if (HitParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		FTransform transform = FTransform();
		transform.SetLocation(GetActorLocation());
		FQuat rotQuaternion = FQuat(rotation);
		transform.SetRotation(rotQuaternion);
		transform.SetScale3D(FVector::OneVector);
		spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, transform, true, EPSCPoolMethod::AutoRelease);
		spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Super::EnemyDeath();
}

void ACardinalEnemyPawn::BurstOverlap()
{
	dynamicWingMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicCockpitMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicShipMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicEngineMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	Super::BurstOverlap();
}

void ACardinalEnemyPawn::SetActive_Implementation(bool IsActive)
{
	Active = IsActive;
	if (IsActive) {
		// Hides visible components
		SetActorHiddenInGame(false);
		// Disables collision components
		SetActorEnableCollision(true);
		// Stops the Actor from ticking
		SetActorTickEnabled(true);
		health = startingHealth;
		rotationDegreesRemaining = 0.0;
		timeToMove = 0.0;
		damageRatioOnBurst = 2.0;
		directionToRotate = FRotator::ZeroRotator;
		currentDirection = ECurrentDirection::VE_Left;
		nextDirection = ECurrentDirection::VE_Left;
		currentYaw = 0.0;
		damageRatio = 1.0;
		dynamicWingMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicCockpitMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicShipMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicEngineMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		// Hides visible components
		SetActorHiddenInGame(true);
		// Disables collision components
		SetActorEnableCollision(false);
		// Stops the Actor from ticking
		SetActorTickEnabled(false);
		playerPawn = nullptr;
		OnEnemyDeathDelegate.Clear();
		OnIncreaseMultiplierDelegate.Clear();
		SetActorLocation(FVector(0.0, 0.0, 10000.0f));
		SetActorRotation(FRotator::ZeroRotator);
	}
}