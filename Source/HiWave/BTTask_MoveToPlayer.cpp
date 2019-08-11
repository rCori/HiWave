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

void UBTTask_MoveToPlayer::OnGameplayTaskActivated(class UGameplayTask &)
{
}


EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAI *CharPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

	AHiWavePawn *Enemy = Cast<AHiWavePawn>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(CharPC->EnemyKeyID));

	if (Enemy) {


		//UNavigationSystem* Nav = UNavigationSystem::GetCurrent(GetWorld());

		//UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), position, NULL);

		AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(CharPC->GetPawn());

		FVector movementDirection = (Enemy->GetActorLocation() - CharPC->GetPawn()->GetActorLocation()).GetSafeNormal();
		enemyPawn->AddMovementInput(movementDirection, 2.0f);

		//CharPC->MoveToActor(Enemy, 1.0f, true, true, true, 0, true);
		//CharPC->MoveToLocation(Enemy->GetActorLocation(), 0.5f, false, false, false, 0, false);
		//FVector NewLocation = ((Enemy->GetActorLocation()- CharPC->GetPawn()->GetActorLocation()).GetSafeNormal() * 3.5) + CharPC->GetPawn()->GetActorLocation();
		//CharPC->GetPawn()->SetActorLocation(NewLocation);
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Succeeded;
}