// Fill out your copyright notice in the Description page of Project Settings.


#include "RedEnemy.h"
#include "EnemyPawn.h"
#include "HiWavePawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "CollidingPawnMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMathLibrary.h"

ARedEnemy::ARedEnemy() : AEnemyPawn() {
	//Create the static mesh for this specific pawn
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	StaticMeshComponentPtr->SetStaticMesh(ShipMesh.Object);

	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	//AIControllerClass = AEnemyAI::StaticClass();

	//Assign bot behavior by grabbing the BehaviorTree object in content
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'"));
	//BotBehavior = BTob.Object;

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;


	gunOffset = FVector(90.0f, 100.0f, 0.0f);

	static ConstructorHelpers::FClassFinder<AActor> redProjectileClassFinder(TEXT("/Game/Blueprint/BP_RedEnemyProjectile"));
	if (redProjectileClassFinder.Succeeded())
	{
		redEnemyProjectile = redProjectileClassFinder.Class;
	}

	health = 100.0;
	speed = 400.0;
	pointsAwarded = 100;
	damageRatio = 1.0;
	burstAwarded = 1.0;
	rotationSpeed = 200.0;
	fireRate = 5.0;
	fireTimer = 0.0;

	yawDifference = MAX_FLT;
	bFacingPlayer = false;
}

void ARedEnemy::Tick(float DeltaTime)
{

	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
	}

	FRotator lookAtRotate = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), playerPawn->GetActorLocation());

	yawDifference = lookAtRotate.Yaw - GetActorRotation().Yaw;
	if (FMath::Abs(yawDifference) < 2.0) {
		yawDifference = MAX_FLT;
		bFacingPlayer = true;
	}
	else {
		bFacingPlayer = false;
	}

	if (bFacingPlayer) {
		FVector newDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		AddMovementInput(newDirection, 1.0f);
	}
	else {
		directionToRotate.Yaw = DeltaTime * rotationSpeed;
		if (yawDifference < 0) {
			directionToRotate.Yaw *= -1;
		}
		AddActorLocalRotation(directionToRotate);
	}

	fireTimer += DeltaTime;

	if (fireTimer >= fireRate) {

		//Get the rotation of the projectile
		const FRotator FireRotation = GetActorRotation();

		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(gunOffset);

		//In order to spawn anything we need reference to the world
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile
			//World->SpawnActor<ARedEnemyProjectile>(SpawnLocation, FireRotation);
			World->SpawnActor<AActor>(redEnemyProjectile, SpawnLocation, FireRotation);
		}

		fireTimer = 0.0;
	}

}

// Called when the game starts or when spawned
void ARedEnemy::BeginPlay()
{

	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto frontMaterial = staticMesh->GetMaterial(0);
	auto sizeMaterial = staticMesh->GetMaterial(1);

	dynamicFrontMaterial = UMaterialInstanceDynamic::Create(frontMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicFrontMaterial);

	dynamicSideMaterial = UMaterialInstanceDynamic::Create(sizeMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicSideMaterial);

	Super::BeginPlay();
}

void ARedEnemy::EnemyDeath()
{
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

void ARedEnemy::BurstOverlap()
{
	dynamicFrontMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicSideMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	damageRatio = 3.0;
}

