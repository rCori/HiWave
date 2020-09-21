// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPool.h"
#include "PoolableActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

// Sets default values
AItemPool::AItemPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemPool::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	if (World) {
		for (const TPair<EPoolableType, TSubclassOf<APoolableActor>>& pair : PoolItemMap)
		{
			EPoolableType currentType = pair.Key;
			TSubclassOf<APoolableActor> typeToSpawn = pair.Value;
			if (typeToSpawn != NULL) {

				int countToSpawn = InitialPooledItemCount[currentType];
				pooledItemCollection.Add(currentType,TArray<APoolableActor*>());
				for (int i = 0; i < countToSpawn; i++) {
					APoolableActor* PoolableActor = GetWorld()->SpawnActor<APoolableActor>(typeToSpawn, FVector().ZeroVector, FRotator().ZeroRotator);
					PoolableActor->SetActive(false);
					pooledItemCollection[currentType].Add(PoolableActor);
				}
			}

		}
	}
	
}

// Called every frame
void AItemPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


APoolableActor* AItemPool::GetPooledObject(EPoolableType type)
{
	if (pooledItemCollection.Contains(type)) {
		TArray<APoolableActor*> poolableActorArray = pooledItemCollection[type];
		for (APoolableActor* PoolableActor : poolableActorArray) {
			if (!PoolableActor->IsActive()) {
				return PoolableActor;
			}
		}
	}
	else {
		pooledItemCollection.Add(type, TArray<APoolableActor*>());
	}
	//There were no active items so generate one and add it to the pool
	APoolableActor* PoolableActor = GetWorld()->SpawnActor<APoolableActor>(PoolItemMap[type], FVector().ZeroVector, FRotator().ZeroRotator);
	PoolableActor->SetActive(false);
	pooledItemCollection[type].Add(PoolableActor);
	return PoolableActor;
}