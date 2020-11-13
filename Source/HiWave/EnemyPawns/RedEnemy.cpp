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
	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	gunOffset = FVector(90.0f, 100.0f, 0.0f);

	static ConstructorHelpers::FClassFinder<AActor> redProjectileClassFinder(TEXT("/Game/Blueprint/BP_RedEnemyProjectile"));
	if (redProjectileClassFinder.Succeeded())
	{
		redEnemyProjectile = redProjectileClassFinder.Class;
	}

	health = startingHealth;
	speed = 400.0;
	pointsAwarded = 100;
	damageRatio = 1.0;
	burstAwarded = 1.0;
	rotationSpeed = 200.0;
	fireRate = 5.0;
	fireTimer = 0.0;
	damageRatioOnBurst = 2.0;

	yawDifference = MAX_FLT;
	bFacingPlayer = false;
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
		if (worldRef == nullptr) {
			worldRef = GetWorld();
			if (worldRef == nullptr) return;
		}

		//rfUWorld* const World = GetWorld();
		if (worldRef != NULL)
		{
			// spawn the projectile
			worldRef->SpawnActor<AActor>(redEnemyProjectile, SpawnLocation, FireRotation);
		}

		fireTimer = 0.0;
	}

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
	Super::BurstOverlap();
}

/*
void ARedEnemy::DeactivateEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("DeactivateEvent"));
	IPoolableObjectInterface::Execute_Deactivate(this);
}

void ARedEnemy::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ARedEnemy::DeactivateEvent, Lifespan, false);
}

bool ARedEnemy::IsActive_Implementation()
{
	return Active;
}

void ARedEnemy::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}
*/

void ARedEnemy::SetActive_Implementation(bool IsActive)
{
	UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation in RedEnemy"));
	Active = IsActive;
	if (IsActive) {
		UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation setting active true"));
		// Hides visible components
		SetActorHiddenInGame(false);
		// Disables collision components
		SetActorEnableCollision(true);
		// Stops the Actor from ticking
		SetActorTickEnabled(true);
		health = startingHealth;
		yawDifference = MAX_FLT;
		bFacingPlayer = false;
		damageRatio = 1.0;
		dynamicFrontMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicSideMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation setting active false"));
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