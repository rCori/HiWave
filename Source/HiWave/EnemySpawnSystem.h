// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPawnSpawner.h"
#include "EnemySpawnSystem.generated.h"

UCLASS()
class HIWAVE_API AEnemySpawnSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn Point Collection")
	TMap<FString, AEnemyPawnSpawner*> SpawnerCollection;

	UFUNCTION(BlueprintCallable)
	void DoSpawn(FString tag);

	UFUNCTION(BlueprintCallable)
	void DoDummySpawning();

	

private:
	FTimerDelegate TimerDelLeft;
	FTimerDelegate TimerDelCenter;
	FTimerDelegate TimerDelRight;
	FTimerHandle TimerHandle1;
	FTimerHandle TimerHandle2;
	FTimerHandle TimerHandle3;
	FTimerHandle TimerHandle4;
	FTimerHandle TimerHandle5;
	FTimerHandle TimerHandle6;



};
