// Fill out your copyright notice in the Description page of Project Settings.


#include "DashingEnemy.h"
#include "EnemyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CollidingPawnMovementComponent.h"
#include "EnemyAI/DashingEnemyAI.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ADashingEnemy::ADashingEnemy() : AEnemyPawn() {

	//Set the default AI controller class.
	//When spawning use this->SpawnDefaultController()
	AIControllerClass = ADashingEnemyAI::StaticClass();

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
}

void ADashingEnemy::BeginPlay() {

	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto frontMaterial = staticMesh->GetMaterial(0);
	auto sizeMaterial = staticMesh->GetMaterial(1);

	dynamicFrontMaterial = UMaterialInstanceDynamic::Create(frontMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicFrontMaterial);

	dynamicSideMaterial = UMaterialInstanceDynamic::Create(sizeMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicSideMaterial);

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
	damageRatio = 2.0;
}