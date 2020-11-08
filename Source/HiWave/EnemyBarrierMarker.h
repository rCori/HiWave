// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBarrierMarker.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EBarrierPosition : uint8
{
	VE_Top		UMETA(DisplayName = "Top"),
	VE_Bottom	UMETA(DisplayName = "Bottom"),
	VE_Left		UMETA(DisplayName = "Left"),
	VE_Right	UMETA(DisplayName = "Right"),
};

UCLASS()
class HIWAVE_API AEnemyBarrierMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyBarrierMarker();

	UFUNCTION(BlueprintCallable)
	EBarrierPosition GetBarrierPosition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EBarrierPosition BarrierPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class UBoxComponent* BarrierArea;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
