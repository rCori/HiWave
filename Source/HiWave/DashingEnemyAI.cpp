// Fill out your copyright notice in the Description page of Project Settings.


#include "DashingEnemyAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyPawn.h"

ADashingEnemyAI::ADashingEnemyAI()
{
	blackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	behaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void ADashingEnemyAI::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyPawn *Char = Cast<AEnemyPawn>(InPawn);

	if (Char && Char->BotBehavior)
	{
		blackboardComp->InitializeBlackboard(*Char->BotBehavior->BlackboardAsset);
		PlayerID = blackboardComp->GetKeyID("Player");
		DashTargetID = blackboardComp->GetKeyID("DashTarget");
		behaviorComp->StartTree(*Char->BotBehavior);
	}
}