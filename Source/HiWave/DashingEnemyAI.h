// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DashingEnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API ADashingEnemyAI : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(transient)
	class UBlackboardComponent *blackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent *behaviorComp;

public:

	ADashingEnemyAI();

	virtual void OnPossess(APawn *InPawn) override;

	uint8 PlayerID;
	uint8 DashTargetID;
};
