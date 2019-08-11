// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPawnSpawner.generated.h"

class UBoxComponent;

UCLASS()
class HIWAVE_API AEnemyPawnSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPawnSpawner();

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	UBoxComponent* BoxComponent;

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DoEnemyPawnSpawn();

};
