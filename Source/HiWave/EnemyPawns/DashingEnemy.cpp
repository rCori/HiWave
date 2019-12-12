// Fill out your copyright notice in the Description page of Project Settings.


#include "DashingEnemy.h"
#include "EnemyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CollidingPawnMovementComponent.h"
#include "EnemyAI/DashingEnemyAI.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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

	health = 10.0;

	speed = 1250.0;
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