// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DashToTarget.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "HiWavePawn.h"
#include "EnemyPawns/EnemyPawn.h"
#include "EnemyAI/DashingEnemyAI.h"


UBTTask_DashToTarget::UBTTask_DashToTarget(const FObjectInitializer& objectInitializer)
{
	bNotifyTick = 1;
	distance = MAX_FLT;
}

void UBTTask_DashToTarget::OnGameplayTaskActivated(class UGameplayTask &)
{
	distance = MAX_FLT;
}



EBTNodeResult::Type UBTTask_DashToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (distance < 50.0) {
		UE_LOG(LogTemp, Warning, TEXT("Setting DashTarget"));
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("DashTargetFound", false);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_DashToTarget::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{

	ADashingEnemyAI *dashingEnemyAI = Cast<ADashingEnemyAI>(OwnerComp.GetAIOwner());
	FVector DashTarget = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(dashingEnemyAI->DashTargetID);

	if (DashTarget != FVector::ZeroVector) {
		AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(dashingEnemyAI->GetPawn());
		FVector movementDirection = (DashTarget - enemyPawn->GetActorLocation());
		distance = movementDirection.Size();
		FVector movementDirectionNormal = movementDirection.GetSafeNormal();

		enemyPawn->AddMovementInput(movementDirectionNormal, 2.0f);

		if (distance < 50.0) {
			UE_LOG(LogTemp, Warning, TEXT("Setting DashTarget"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			distance = MAX_FLT;
		}
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}