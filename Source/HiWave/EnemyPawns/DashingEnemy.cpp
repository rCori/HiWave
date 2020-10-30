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

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = startingHealth;
	speed = 1250.0;
	pointsAwarded = 50;
	damageRatio = 1.0;
	burstAwarded = 0.5;
	damageRatioOnBurst = 1.5;
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
	Super::BurstOverlap();
}

/*
void ADashingEnemy::DeactivateEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("DeactivateEvent"));
	IPoolableObjectInterface::Execute_Deactivate(this);
}

void ADashingEnemy::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ADashingEnemy::DeactivateEvent, Lifespan, false);
}

bool ADashingEnemy::IsActive_Implementation()
{
	return Active;
}

void ADashingEnemy::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}
*/

void ADashingEnemy::SetActive_Implementation(bool IsActive)
{
	UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation in ADashingEnemy"));
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
		yawDifference = 0.0f;
		bFacingPlayer = false;
		directionToRotate = FRotator::ZeroRotator;
		dashDirection = FVector::ZeroVector;
		dashTarget = FVector::ZeroVector;
		damageRatio = 1.0;
		dynamicFrontMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicSideMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicEngineMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);

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