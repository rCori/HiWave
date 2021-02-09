// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemPool.h"
#include "PoolableActor.h"
#include "PoolableObjectInterface.h"
#include "GameModes/HiWaveGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

// Sets default values
AItemPool::AItemPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	poolItemLocation = FVector(0, 0, 10000.0f);
	referenceCount = 0;
	totalReferences = 1;
}

// Called when the game starts or when spawned
void AItemPool::BeginPlay()
{
	Super::BeginPlay();
	if (hiWaveGameMode == nullptr) {
		hiWaveGameMode = Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode());
		hiWaveGameMode->OnDestroyAllEnemies.AddDynamic(this, &AItemPool::DisableEnemyTypesOnDestroyEvent);
	}
}

void AItemPool::IncreaseReferenceCountToSpawn() {
	referenceCount++;
	if (referenceCount == totalReferences) {
		DoInitialSpawns();
	}
}

void AItemPool::DoInitialSpawns() {
	UWorld* const World = GetWorld();
	if (World) {
		for (const TPair<EPoolableType, TSubclassOf<UObject>>& pair : PoolItemMap)
		{
			EPoolableType currentType = pair.Key;
			TSubclassOf<UObject> typeToSpawn = pair.Value;
			bool implementsInterface = typeToSpawn.Get()->GetClass()->ImplementsInterface(UPoolableObjectInterface::StaticClass());
			if (typeToSpawn != NULL) {
				int countToSpawn = InitialPooledItemCount[currentType];
				pooledItemCollection.Add(currentType, TArray<IPoolableObjectInterface*>());
				for (int i = 0; i < countToSpawn; i++) {
					FActorSpawnParameters ActorSpawnParameters;
					ActorSpawnParameters.Owner = this;
					ActorSpawnParameters.Instigator = Instigator;
					ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					IPoolableObjectInterface* PoolableActor = GetWorld()->SpawnActor<IPoolableObjectInterface>(typeToSpawn, poolItemLocation, FRotator().ZeroRotator, ActorSpawnParameters);
					UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
					IPoolableObjectInterface::Execute_SetActive(PoolableActorObject, false);
					pooledItemCollection[currentType].Add(PoolableActor);
				}
			}
		}
		InitialItemSpawnsFinished.Broadcast();
	}
}

void AItemPool::DisableAllOfType(EPoolableType type)
{
	TArray<IPoolableObjectInterface*> poolableActorArray = pooledItemCollection[type];
	//for (APoolableActor* PoolableActor : poolableActorArray) {
	for (IPoolableObjectInterface* PoolableActor : poolableActorArray) {
		UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
		if (IPoolableObjectInterface::Execute_IsActive(PoolableActorObject)) {
			IPoolableObjectInterface::Execute_SetActive(PoolableActorObject,false);
		}
	}
}

void AItemPool::DisableEnemyTypesOnDestroyEvent() {
	DisableAllOfType(EPoolableType::VE_RedEnemyBullet);
	DisableAllOfType(EPoolableType::VE_SkullMineWeapon);
}

// Called every frame
/*
void AItemPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/


IPoolableObjectInterface* AItemPool::GetPooledObject(EPoolableType type)
{
	//UE_LOG(LogTemp, Warning, TEXT("GetPooledObject"));
	if (pooledItemCollection.Contains(type)) {
		//TArray<APoolableActor*> poolableActorArray = pooledItemCollection[type];
		TArray<IPoolableObjectInterface*> poolableActorArray = pooledItemCollection[type];
		//for (APoolableActor* PoolableActor : poolableActorArray) {
		for (IPoolableObjectInterface* PoolableActor : poolableActorArray) {
			UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
			if (!IPoolableObjectInterface::Execute_IsActive(PoolableActorObject)) {
				//UE_LOG(LogTemp, Warning, TEXT("Returning item from the pool"));
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
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;
	ActorSpawnParameters.Instigator = Instigator;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	IPoolableObjectInterface* PoolableActor = GetWorld()->SpawnActor<IPoolableObjectInterface>(PoolItemMap[type], poolItemLocation, FRotator().ZeroRotator, ActorSpawnParameters);
	
	UObject* PoolableActorObject = Cast<UObject>(PoolableActor);
	//PoolableActor->SetActive(false);
	IPoolableObjectInterface::Execute_SetActive(PoolableActorObject, false);
	pooledItemCollection[type].Add(PoolableActor);
	UE_LOG(LogTemp, Warning, TEXT("Making a new item and adding it to the pool"));
	return PoolableActor;

}