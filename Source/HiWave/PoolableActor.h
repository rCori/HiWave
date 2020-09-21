// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableActor.generated.h"

UCLASS()
class HIWAVE_API APoolableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoolableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetLifeSpan(float InLifespan) override;
	virtual void SetActive(bool IsActive);
	bool IsActive();

protected:
	float Lifespan = 5.0f;
	bool Active;
	void Deactivate();
	FTimerHandle LifespanTimer;
};
