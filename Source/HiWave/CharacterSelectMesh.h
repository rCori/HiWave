// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CharacterTypes.h"
#include "CharacterSelectMesh.generated.h"

UCLASS()
class HIWAVE_API ACharacterSelectMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterSelectMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECharacterTypes, UStaticMesh*> CharacterMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECharacterTypes, FTransform> CharacterMeshLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterTypes CurrentCharacterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float spinRate;

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void ChangeCharacterMesh(ECharacterTypes characterType);


protected:
	UStaticMeshComponent* currentMesh;

};
