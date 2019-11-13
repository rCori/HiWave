// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RedEnemyProjectileFire.generated.h"

/**
 * 
 */
class ARedEnemyProjectile;
UCLASS()
class HIWAVE_API UBTTask_RedEnemyProjectileFire : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_RedEnemyProjectileFire(const FObjectInitializer& objectInitializer);

	virtual void OnGameplayTaskActivated(class UGameplayTask &) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UClass *redEnemyProjectile;
	//ARedEnemyProjectile *redEnemyProjectile;

	FVector gunOffset;

};
