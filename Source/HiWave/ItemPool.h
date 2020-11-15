// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "HiWaveProjectile.h"
#include "PoolableTypes.h"
#include "PoolableObjectInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitialItemSpawnsFinished);

UCLASS()
class HIWAVE_API AItemPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, Category = "ObjectPooler")
	//TSubclassOf<AHiWaveProjectile> PooledObjectSubclass;

	UFUNCTION()
	void IncreaseReferenceCountToSpawn();

	UFUNCTION()
	void DoInitialSpawns();

	/*
	UFUNCTION()
	APoolableActor* GetPooledObject(EPoolableType type);
	*/
	IPoolableObjectInterface* GetPooledObject(EPoolableType type);

	/*
	UPROPERTY(EditDefaultsOnly, Category = "ObjectPooler")
	TMap<EPoolableType, TSubclassOf<APoolableActor>> PoolItemMap;
	*/
	UPROPERTY(EditDefaultsOnly, Category = "ObjectPooler")
	TMap<EPoolableType, TSubclassOf<UObject>> PoolItemMap;

	UPROPERTY(EditDefaultsOnly, Category = "ObjectPooler")
	TMap<EPoolableType, int> InitialPooledItemCount;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FInitialItemSpawnsFinished InitialItemSpawnsFinished;

private:

	FVector poolItemLocation;

	TMap<EPoolableType, TArray<IPoolableObjectInterface*>> pooledItemCollection;

	int referenceCount;
	int totalReferences;

};
