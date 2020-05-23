// Fill out your copyright notice in the Description page of Project Settings.


#include "DashingEnemy.h"
#include "EnemyPawn.h"
#include "HiWavePawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CollidingPawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"

ADashingEnemy::ADashingEnemy() : AEnemyPawn() {

	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	//AIControllerClass = ADashingEnemyAI::StaticClass();

	//Assign bot behavior by grabbing the BehaviorTree object in content
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'"));
	//BotBehavior = BTob.Object;

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = 20.0;
	speed = 1250.0;
	pointsAwarded = 50;
	damageRatio = 1.0;
	burstAwarded = 0.5;
}

void ADashingEnemy::Tick(float DeltaTime)
{
	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
	}

	if (!bFacingPlayer) {
		FRotator lookAtRotate = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), playerPawn->GetActorLocation());

		yawDifference = lookAtRotate.Yaw - GetActorRotation().Yaw;
		if (FMath::Abs(yawDifference) < 2.0) {
			yawDifference = MAX_FLT;
			bFacingPlayer = true;
			dashTarget = playerPawn->GetActorLocation();
			dashDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
		else {
			directionToRotate.Yaw = DeltaTime * 200.0;
			if (yawDifference < 0) {
				directionToRotate.Yaw *= -1;
			}
			AddActorLocalRotation(directionToRotate);
			bFacingPlayer = false;
		}
	}
	else {
		FVector distanceVec = (dashTarget - GetActorLocation());

		float distance = distanceVec.Size();
		if (distance < 10.0) {
			distance = MAX_FLT;
			bFacingPlayer = false;
		}
		else {
			AddMovementInput(dashDirection, 1.0f);
		}
	}
}

void ADashingEnemy::BeginPlay() {

	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto frontMaterial = staticMesh->GetMaterial(0);
	auto sizeMaterial = staticMesh->GetMaterial(1);
	auto engineMaterial = staticMesh->GetMaterial(3);

	dynamicFrontMaterial = UMaterialInstanceDynamic::Create(frontMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicFrontMaterial);

	dynamicSideMaterial = UMaterialInstanceDynamic::Create(sizeMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicSideMaterial);

	dynamicEngineMaterial = UMaterialInstanceDynamic::Create(engineMaterial, NULL);
	staticMesh->SetMaterial(3, dynamicEngineMaterial);

	Super::BeginPlay();
}

void ADashingEnemy::EnemyDeath()
{
	if (HitParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		FTransform transform = FTransform();
		transform.SetLocation(GetActorLocation());
		FQuat rotQuaternion = FQuat(rotation);
		transform.SetRotation(rotQuaternion);
		transform.SetScale3D(FVector::OneVector);
		//spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), rotation);
		spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, transform, true, EPSCPoolMethod::AutoRelease);
		spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	Super::EnemyDeath();
}

void ADashingEnemy::BurstOverlap()
{
	dynamicFrontMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicSideMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicEngineMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	damageRatio = 2.0;
}