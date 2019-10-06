// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CircularMoveAroundPoint.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "EnemyAI.h"
#include "HiWavePawn.h"
#include "EnemyPawn.h"

UBTTask_CircularMoveAroundPoint::UBTTask_CircularMoveAroundPoint(const FObjectInitializer& objectInitializer) 
	: Super(objectInitializer)
{
	bNotifyTick = 1;


	currentMovementDirection = FVector(1, 0, 0);
	arcCurrTime = 0;
	arcTimer = 1.0;
}

void UBTTask_CircularMoveAroundPoint::OnGameplayTaskActivated(class UGameplayTask &)
{
	
}


EBTNodeResult::Type UBTTask_CircularMoveAroundPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AEnemyAI *CharPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

	FVector PointToRotate = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(CharPC->RotatePointID);


	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(CharPC->GetPawn());
	/*
	//FVector movementDirection = (Enemy->GetActorLocation() - CharPC->GetPawn()->GetActorLocation()).GetSafeNormal();
	//enemyPawn->AddMovementInput(movementDirection, 2.0f);
	FVector distance = (PointToRotate - CharPC->GetPawn()->GetActorLocation()).GetSafeNormal();
	//UE_LOG(LogTemp, Warning, TEXT("distance %s"), *distance.ToString());

	float angleInRadians = acosf(FVector::DotProduct(distance, FVector::ForwardVector));
	float angleInDegrees = FMath::RadiansToDegrees(angleInRadians);
	UE_LOG(LogTemp, Warning, TEXT("angleInDegrees %f"), angleInDegrees);

	float xCircle = FMath::Cos(angleInRadians);
	float yCircle = FMath::Sin(angleInRadians);
		
	FVector trigLocation(xCircle, yCircle, 0.0);
	//UE_LOG(LogTemp, Warning, TEXT("trigLocation %s"), *trigLocation.ToString());
	FVector movementDirection = FVector::CrossProduct(trigLocation, FVector::UpVector);
	UE_LOG(LogTemp, Warning, TEXT("movementDirection %s"), *movementDirection.ToString());

	enemyPawn->AddMovementInput(movementDirection, 1.0f);
	return EBTNodeResult::InProgress;
	*/
	UE_LOG(LogTemp, Warning, TEXT("currentMovementDirection %s"), *currentMovementDirection.ToString());
	enemyPawn->AddMovementInput(currentMovementDirection, 1.0f);
	return EBTNodeResult::InProgress;
}

void UBTTask_CircularMoveAroundPoint::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	//UE_LOG(LogTemp, Warning, TEXT("DeltaSeconds %f"), DeltaSeconds);

	arcCurrTime += DeltaSeconds;


	float moveX = currentMovementDirection.X + (DeltaSeconds * -2.0f);
	float moveY = currentMovementDirection.Y + (DeltaSeconds * 0.75f);

	currentMovementDirection = FVector(moveX, moveY, 0).GetSafeNormal();

	if (arcCurrTime >= arcTimer) {
		arcCurrTime = 0.0f;
		currentMovementDirection = FVector(1, 0, 0);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
