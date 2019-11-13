// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RedEnemyProjectileFire.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyPawn.h"
#include "EnemyAI.h"
#include "RedEnemyProjectile.h"
#include "Engine/Blueprint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

UBTTask_RedEnemyProjectileFire::UBTTask_RedEnemyProjectileFire(const FObjectInitializer& objectInitializer) {
	bNotifyTick = 1;

	gunOffset = FVector(90.0f, 0.0f, 0.0f);

	static ConstructorHelpers::FObjectFinder<UBlueprint> RedProjectileBlueprint(TEXT("Blueprint'/Game/Blueprint/BP_RedEnemyProjectile.BP_RedEnemyProjectile'"));
	redEnemyProjectile = (UClass*)RedProjectileBlueprint.Object->GeneratedClass;

}

void UBTTask_RedEnemyProjectileFire::OnGameplayTaskActivated(UGameplayTask &)
{

}

EBTNodeResult::Type UBTTask_RedEnemyProjectileFire::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	float _shotTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("ShotTimer");
	if (_shotTimer > 1.0) {
		UE_LOG(LogTemp, Warning, TEXT("Red enemy is shooting"));
		_shotTimer = 0.0;
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat("ShotTimer", _shotTimer);
		//Cast AI from owner so we can get reference to the pawn itself
		AEnemyAI *enemyAI = Cast<AEnemyAI>(OwnerComp.GetAIOwner());

		if (enemyAI) {
			AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(enemyAI->GetPawn());

			//Get the rotation of the projectile
			const FRotator FireRotation = enemyPawn->GetActorRotation();

			//const FVector SpawnLocation = enemyPawn->GetActorLocation();

			const FVector SpawnLocation = enemyPawn->GetActorLocation() + FireRotation.RotateVector(gunOffset);

			//In order to spawn anything we need reference to the world
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				//World->SpawnActor<ARedEnemyProjectile>(SpawnLocation, FireRotation);
				World->SpawnActor<AActor>(redEnemyProjectile,SpawnLocation, FireRotation);
			}

			return EBTNodeResult::Succeeded;
		}
		else {
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::InProgress;

	
}

void UBTTask_RedEnemyProjectileFire::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	float _shotTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("ShotTimer");
	_shotTimer += DeltaSeconds;
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat("ShotTimer", _shotTimer);
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
