// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolableActor.h"
#include "TimerManager.h"

// Sets default values
APoolableActor::APoolableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APoolableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APoolableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APoolableActor::SetLifeSpan(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &APoolableActor::Deactivate, Lifespan, false);
}

void APoolableActor::SetActive(bool IsActive)
{
	Active = IsActive;
}

bool APoolableActor::IsActive()
{
	return Active;
}

void APoolableActor::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}

