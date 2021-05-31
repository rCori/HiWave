// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectMesh.h"

// Sets default values
ACharacterSelectMesh::ACharacterSelectMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	currentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RootComponent = currentMesh;
	if (CharacterMeshes.Contains(CurrentCharacterType)) {
		currentMesh->SetStaticMesh(CharacterMeshes[CurrentCharacterType]);
		SetActorTransform(CharacterMeshLocations[CurrentCharacterType]);
	}

}

// Called when the game starts or when spawned
void ACharacterSelectMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterSelectMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator currentRotation = currentMesh->GetRelativeTransform().Rotator();
	currentRotation.Add(0, DeltaTime*spinRate, 0);
	currentMesh->SetRelativeRotation(currentRotation);
}

void ACharacterSelectMesh::ChangeCharacterMesh(ECharacterTypes characterType)
{
	if (CharacterMeshes.Contains(characterType)) {
		CurrentCharacterType = characterType;
		currentMesh->SetStaticMesh(CharacterMeshes[CurrentCharacterType]);
		SetActorTransform(CharacterMeshLocations[CurrentCharacterType]);
	}
}
