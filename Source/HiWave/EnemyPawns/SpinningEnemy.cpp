// Fill out your copyright notice in the Description page of Project Settings.


#include "SpinningEnemy.h"
#include "EnemyPawns/EnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "CollidingPawnMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "Math/UnrealMathUtility.h"


ASpinningEnemy::ASpinningEnemy() : AEnemyPawn() {
	
	ArmBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HitDetectionTwo"));
	ArmBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPawn::OnOverlap);
	ArmBoxComponent->SetupAttachment(RootComponent);

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = 40.0;
	speed = 100.0;
	pointsAwarded = 10;
	damageRatio = 1.0;
	burstAwarded = 0.2;

	currentMovementDirection = FVector(1, 0, 0);
	arcCurrTime = 0;
	arcTimer = 1.0;
	circleCounter = 0.0;
	PrimaryActorTick.bCanEverTick = true;
	circleNumber = 1.0;
	rotationSpeed = 1.0;
	damageRatioOnBurst = 2.0;
}

void ASpinningEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	circleCounter += (DeltaTime*circleNumber);
	if (circleCounter > 2.0 * PI) { 
		circleCounter = (circleCounter - (2.0* PI));
	}
	moveX = FMath::Cos(circleCounter);
	moveY = FMath::Sin(circleCounter);

	newMovementDirection = FVector(moveX, moveY, 0);

	AddMovementInput(newMovementDirection, 1.0f);

	NewRotation = FRotator(0.0f, rotationSpeed*DeltaTime, 0.0f);
	QuatRotation = FQuat(NewRotation);
	AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
}


// Called when the game starts or when spawned
void ASpinningEnemy::BeginPlay()
{
	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto bodyMaterial = staticMesh->GetMaterial(0);
	auto armMaterial = staticMesh->GetMaterial(1);

	dynamicBodyMaterial = UMaterialInstanceDynamic::Create(bodyMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicBodyMaterial);

	dynamicArmMaterial = UMaterialInstanceDynamic::Create(armMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicArmMaterial);

	Super::BeginPlay();
}

void ASpinningEnemy::EnemyDeath() {
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

void ASpinningEnemy::BurstOverlap() {
	dynamicBodyMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicArmMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	Super::BurstOverlap();
}