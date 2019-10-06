// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "EnemyAI.h"
#include "HiWavePawn.h"
#include "EnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"

void UBTTask_MoveToPlayer::OnGameplayTaskActivated(class UGameplayTask &)
{
}


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAI *CharPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

	AHiWavePawn *Enemy = Cast<AHiWavePawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(CharPC->EnemyKeyID));

	if (Enemy) {


		AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(CharPC->GetPawn());

		FVector movementDirection = (Enemy->GetActorLocation() - CharPC->GetPawn()->GetActorLocation()).GetSafeNormal();
		enemyPawn->AddMovementInput(movementDirection, 2.0f);

		FRotator lookAtRotate = UKismetMathLibrary::FindLookAtRotation(CharPC->GetPawn()->GetActorLocation(), Enemy->GetActorLocation());
		enemyPawn->SetActorRotation(lookAtRotate);

		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Succeeded;
}