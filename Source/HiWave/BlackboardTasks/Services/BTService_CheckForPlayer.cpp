// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_CheckForPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "HiWavePawn.h"
#include "EnemyAI/EnemyAI.h"
#include "EnemyAI/DashingEnemyAI.h"
#include "EnemyPawns/EnemyPawn.h"


UBTService_CheckForPlayer::UBTService_CheckForPlayer()
{
	bCreateNodeInstance = true;
}


void UBTService_CheckForPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyAI *EnemyPC = Cast<AEnemyAI>(OwnerComp.GetAIOwner());
	
	ADashingEnemyAI *DashingEnemyPC = Cast<ADashingEnemyAI>(OwnerComp.GetAIOwner());

	if (EnemyPC) {
		AHiWavePawn *Enemy = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if (Enemy)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(EnemyPC->EnemyKeyID, Enemy);
		}
	}
	else if (DashingEnemyPC) {
		AHiWavePawn *Player = Cast<AHiWavePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if (Player)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(DashingEnemyPC->PlayerID, Player);
		}
	}
}

void UBTService_CheckForPlayer::OnGameplayTaskActivated(class UGameplayTask &)
{
}

void UBTService_CheckForPlayer::OnGameplayTaskDeactivated(class UGameplayTask &)
{
}