// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemy.h"
#include "EnemyPawn.h"
#include "PlayerPawns/HiWavePawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "CollidingPawnMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABasicEnemy::ABasicEnemy() : AEnemyPawn() {
	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = startingHealth;
	MaxSpeed = 500.0;
	speed = MaxSpeed;
	pointsAwarded = 10;
	damageRatio = 1.0;
	burstAwarded = 0.2;
	damageRatioOnBurst = 2.0;
	UpdateRate = 0.2;
}

void ABasicEnemy::Tick(float DeltaTime)
{
	if (!Active) return;
	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
	}

	updateTimer += DeltaTime;
	if (updateTimer > UpdateRate) {
		newDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		
		//Apply constant acceleration to your speed
		CurrentSpeed += newDirection * Acceleration * DeltaTime;

		//Constantly subtract friction from your speed
		CurrentSpeed -= CurrentSpeed * Friction * DeltaTime;

		//Limit your speed to a maximum speed;
		if (FMath::Abs(CurrentSpeed.Size()) > MaxSpeed) {
			CurrentSpeed = newDirection * MaxSpeed * DeltaTime;
		}
		speed = FMath::Abs(CurrentSpeed.Size());
		AddMovementInput(CurrentSpeed.GetSafeNormal(), 1.0f);
	}

}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay()
{
	//Try to get player pawn
	playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto material = staticMesh->GetMaterial(0);

	dynamicMaterial = UMaterialInstanceDynamic::Create(material, NULL);
	staticMesh->SetMaterial(0, dynamicMaterial);

	Super::BeginPlay();

}

void ABasicEnemy::EnemyDeath()
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


void ABasicEnemy::BurstOverlap()
{
	dynamicMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	Super::BurstOverlap();
}


void ABasicEnemy::SetActive_Implementation(bool IsActive)
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
		damageRatio = 1.0;
		dynamicMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
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