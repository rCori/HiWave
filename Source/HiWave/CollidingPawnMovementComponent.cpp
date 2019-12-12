// Fill out your copyright notice in the Description page of Project Settings.


#include "CollidingPawnMovementComponent.h"
#include "EnemyPawns/EnemyPawn.h"


void UCollidingPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if (!enemyPawnRef) {
		enemyPawnRef = Cast<AEnemyPawn>(PawnOwner);
	}

	// Get (and then clear) the movement vector that we set in ACollidingPawn::Tick
	float moveSpeed = enemyPawnRef != nullptr ? enemyPawnRef->GetSpeed() : 0.0f;
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * moveSpeed;
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
};
