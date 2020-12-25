// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBarrierMarker.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemyBarrierMarker::AEnemyBarrierMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BarrierArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BarrierArea"));
	BarrierArea->SetupAttachment(RootComponent);
}

EBarrierPosition AEnemyBarrierMarker::GetBarrierPosition() const
{
	return BarrierPosition;
}

// Called when the game starts or when spawned
void AEnemyBarrierMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBarrierMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

