// Fill out your copyright notice in the Description page of Project Settings.


#include "XEnemyPawn.h"
#include "HiWavePawn.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBarrierMarker.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"

AXEnemyPawn::AXEnemyPawn() : AEnemyPawn() {
	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	BarrierCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BarrierDetection"));
	BarrierCollisionComponent->SetupAttachment(RootComponent);
	BarrierCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AXEnemyPawn::OnBarrierOverlap);

	health = startingHealth;
	speed = 500.0;
	pointsAwarded = 2;
	damageRatio = 1.0;
	burstAwarded = 0.05;
	damageRatioOnBurst = 2.0;
}

// Called when the game starts or when spawned
void AXEnemyPawn::BeginPlay()
{
	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto material = staticMesh->GetMaterial(0);

	dynamicMaterial = UMaterialInstanceDynamic::Create(material, NULL);
	staticMesh->SetMaterial(0, dynamicMaterial);

	Super::BeginPlay();
}


void AXEnemyPawn::Tick(float DeltaTime)
{
	if (playerPawn == nullptr) {
		playerPawn = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//If we could not find a player pawn then just leave early
		if (playerPawn == nullptr) return;
		CurrentDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	AddMovementInput(CurrentDirection, 1.0f);
	NewRotation = FRotator(0.0f, rotationSpeed*DeltaTime, 0.0f);
	QuatRotation = FQuat(NewRotation);
	AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);


	/*
	FVector newDirection = (playerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	AddMovementInput(newDirection, 1.0f);

	NewRotation = FRotator(0.0f, rotationSpeed*DeltaTime, 0.0f);
	QuatRotation = FQuat(NewRotation);
	AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
	*/

}

void AXEnemyPawn::EnemyDeath()
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

void AXEnemyPawn::BurstOverlap()
{
	dynamicMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	Super::BurstOverlap();
}

void AXEnemyPawn::SetActive_Implementation(bool IsActive)
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


void AXEnemyPawn::OnBarrierOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AEnemyBarrierMarker *enemyBarrierMarker = Cast<AEnemyBarrierMarker>(OtherActor);
	if (enemyBarrierMarker != nullptr) {
		EBarrierPosition barrierPosition = enemyBarrierMarker->GetBarrierPosition();
		switch (barrierPosition) {
		case EBarrierPosition::VE_Bottom:
		case EBarrierPosition::VE_Top:
			CurrentDirection.Set(-CurrentDirection.X, CurrentDirection.Y, CurrentDirection.Z);
			break;
		case EBarrierPosition::VE_Left:
		case EBarrierPosition::VE_Right:
			CurrentDirection.Set(CurrentDirection.X, -CurrentDirection.Y, CurrentDirection.Z);
			break;
		}
	}

}