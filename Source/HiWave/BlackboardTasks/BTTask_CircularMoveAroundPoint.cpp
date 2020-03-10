// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CircularMoveAroundPoint.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "EnemyAI/EnemyAI.h"
#include "HiWavePawn.h"
#include "EnemyPawns/EnemyPawn.h"

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
	//UE_LOG(LogTemp, Warning, TEXT("UBTTask_CircularMoveAroundPoint ExecuteTask"));
	AEnemyAI *CharPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

	FVector PointToRotate = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(CharPC->RotatePointID);


	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(CharPC->GetPawn());
	//UE_LOG(LogTemp, Warning, TEXT("currentMovementDirection %s"), *currentMovementDirection.ToString());
	enemyPawn->AddMovementInput(currentMovementDirection, 1.0f);
	return EBTNodeResult::InProgress;
}

void UBTTask_CircularMoveAroundPoint::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	arcCurrTime += DeltaSeconds;


	float moveX = currentMovementDirection.X + (DeltaSeconds * -2.0f);
	float moveY = currentMovementDirection.Y + (DeltaSeconds * 0.75f);

	FVector newMovementDirection = FVector(moveX, moveY, 0).GetSafeNormal();



	if (arcCurrTime >= arcTimer) {
		arcCurrTime = 0.0f;
		newMovementDirection = FVector(1, 0, 0);
	}

	currentMovementDirection = newMovementDirection;

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
