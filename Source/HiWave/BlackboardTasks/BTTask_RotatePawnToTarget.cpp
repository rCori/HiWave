// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotatePawnToTarget.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyAI/EnemyAI.h"
#include "EnemyAI/DashingEnemyAI.h"
#include "EnemyPawns/EnemyPawn.h"
#include "HiWavePawn.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotatePawnToTarget::UBTTask_RotatePawnToTarget(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	bNotifyTick = 1;
	yawDifference = MAX_FLT;
}

void UBTTask_RotatePawnToTarget::OnGameplayTaskActivated(class UGameplayTask &)
{
	directionToRotate = FRotator::ZeroRotator;
	yawDifference = MAX_FLT;
}


EBTNodeResult::Type UBTTask_RotatePawnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADashingEnemyAI *CharPC = Cast<ADashingEnemyAI>(OwnerComp.GetAIOwner());
	AHiWavePawn *player = Cast<AHiWavePawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(CharPC->PlayerID));
	if (player != nullptr && FMath::Abs(yawDifference) < 0.5) {
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("DashTarget", player->GetActorLocation());
		yawDifference = MAX_FLT;
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_RotatePawnToTarget::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	ADashingEnemyAI *CharPC = Cast<ADashingEnemyAI>(OwnerComp.GetAIOwner());
	AHiWavePawn *player = Cast<AHiWavePawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(CharPC->PlayerID));

	if (player != nullptr) {
		AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(CharPC->GetPawn());
		FRotator lookAtRotate = UKismetMathLibrary::FindLookAtRotation(CharPC->GetPawn()->GetActorLocation(), player->GetActorLocation());
		yawDifference = lookAtRotate.Yaw - CharPC->GetPawn()->GetActorRotation().Yaw;
		if (FMath::Abs(yawDifference) < 0.5) {
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("DashTarget", player->GetActorLocation());
			yawDifference = MAX_FLT;
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		directionToRotate.Yaw = DeltaSeconds * 200.0;
		if (yawDifference < 0) {
			directionToRotate.Yaw *= -1;
		}
		enemyPawn->AddActorLocalRotation(directionToRotate);
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}