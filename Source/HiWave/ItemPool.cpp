// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPool.h"
#include "PoolableActor.h"
#include "PoolableObjectInterface.h"
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
		//for (const TPair<EPoolableType, TSubclassOf<APoolableActor>>& pair : PoolItemMap)
		for (const TPair<EPoolableType, TSubclassOf<UObject>>& pair : PoolItemMap)
		{
			EPoolableType currentType = pair.Key;
			//TSubclassOf<APoolableActor> typeToSpawn = pair.Value;
			TSubclassOf<UObject> typeToSpawn = pair.Value;
			bool implementsInterface = typeToSpawn.Get()->GetClass()->ImplementsInterface(UPoolableObjectInterface::StaticClass());
			if (typeToSpawn != NULL && implementsInterface) {
				int countToSpawn = InitialPooledItemCount[currentType];
				pooledItemCollection.Add(currentType,TArray<IPoolableObjectInterface*>());
				for (int i = 0; i < countToSpawn; i++) {
					IPoolableObjectInterface* PoolableActor = GetWorld()->SpawnActor<IPoolableObjectInterface>(typeToSpawn, FVector().ZeroVector, FRotator().ZeroRotator);
					UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
					IPoolableObjectInterface::Execute_SetActive(PoolableActorObject,false);
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


IPoolableObjectInterface* AItemPool::GetPooledObject(EPoolableType type)
{
	if (pooledItemCollection.Contains(type)) {
		//TArray<APoolableActor*> poolableActorArray = pooledItemCollection[type];
		TArray<IPoolableObjectInterface*> poolableActorArray = pooledItemCollection[type];
		//for (APoolableActor* PoolableActor : poolableActorArray) {
		for (IPoolableObjectInterface* PoolableActor : poolableActorArray) {
			UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
			if (!IPoolableObjectInterface::Execute_IsActive(PoolableActorObject)) {
				return PoolableActor;
			}
		}
	}
	else {
		//pooledItemCollection.Add(type, TArray<APoolableActor*>());
		pooledItemCollection.Add(type, TArray<IPoolableObjectInterface*>());
	}
	//There were no active items so generate one and add it to the pool
	//APoolableActor* PoolableActor = GetWorld()->SpawnActor<APoolableActor>(PoolItemMap[type], FVector().ZeroVector, FRotator().ZeroRotator);
	IPoolableObjectInterface* PoolableActor = GetWorld()->SpawnActor<IPoolableObjectInterface>(PoolItemMap[type], FVector().ZeroVector, FRotator().ZeroRotator);
	UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
	//PoolableActor->SetActive(false);
	IPoolableObjectInterface::Execute_SetActive(PoolableActorObject, false);
	pooledItemCollection[type].Add(PoolableActor);
	return PoolableActor;
}