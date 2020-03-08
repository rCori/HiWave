// Fill out your copyright notice in the Description page of Project Settings.


#include "SpinningEnemy.h"
#include "EnemyPawns/EnemyPawn.h"
#include "CollidingPawnMovementComponent.h"
#include "Components/BoxComponent.h"
#include "EnemyAI/EnemyAI.h"


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

	PrimaryActorTick.bCanEverTick = true;

}

void ASpinningEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float moveX = currentMovementDirection.X + (DeltaTime * -2.0f);
	float moveY = currentMovementDirection.Y + (DeltaTime * 0.75f);

	FVector newMovementDirection = FVector(moveX, moveY, 0).GetSafeNormal();

	if (arcCurrTime >= arcTimer) {
		arcCurrTime = 0.0f;
		newMovementDirection = FVector(1, 0, 0);
	}

	currentMovementDirection = newMovementDirection;
	AddMovementInput(currentMovementDirection, 1.0f);
}

void ASpinningEnemy::EnemyDeath() {
	Super::EnemyDeath();
}

void ASpinningEnemy::BurstOverlap() {
	damageRatio = 2.0;
}