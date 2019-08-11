// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "EnemyAI.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API AEnemyAI : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(transient)
	class UBlackboardComponent *blackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent *behaviorComp;

public:
	
	AEnemyAI();

	virtual void OnPossess(APawn *InPawn) override;

	uint8 EnemyKeyID;
};
