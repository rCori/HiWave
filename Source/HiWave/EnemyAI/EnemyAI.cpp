// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyPawns/EnemyPawn.h"




AEnemyAI::AEnemyAI()
{
	blackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	behaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AEnemyAI::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyPawn *Char = Cast<AEnemyPawn>(InPawn);

	if (Char && Char->BotBehavior)
	{
		blackboardComp->InitializeBlackboard(*Char->BotBehavior->BlackboardAsset);
		EnemyKeyID = blackboardComp->GetKeyID("Target");
		RotatePointID = blackboardComp->GetKeyID("RotatePoint");
		behaviorComp->StartTree(*Char->BotBehavior);
	}
}