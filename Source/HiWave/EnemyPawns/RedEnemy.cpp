// Fill out your copyright notice in the Description page of Project Settings.


#include "RedEnemy.h"
#include "EnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "CollidingPawnMovementComponent.h"
#include "EnemyAI/EnemyAI.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ARedEnemy::ARedEnemy() : AEnemyPawn() {
	//Create the static mesh for this specific pawn
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	StaticMeshComponentPtr->SetStaticMesh(ShipMesh.Object);

	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	AIControllerClass = AEnemyAI::StaticClass();

	//Assign bot behavior by grabbing the BehaviorTree object in content
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTob(TEXT("BehaviorTree'/Game/AI/EnemyPawnBT.EnemyPawnBT'"));
	BotBehavior = BTob.Object;

	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	health = 100.0;
	speed = 400.0;
	pointsAwarded = 100;
	damageRatio = 1.0;
	burstAwarded = 1.0;
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

