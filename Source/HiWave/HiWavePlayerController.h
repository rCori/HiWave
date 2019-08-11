// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HiWavePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API AHiWavePlayerController : public APlayerController
{
	GENERATED_BODY()

	//Constructor
	AHiWavePlayerController();
private:
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	/* The X location in screen space of the mouse */
	UPROPERTY(Category = Gameplay, EditAnywhere)
	float mouseX;

	/* The Y location in screen space of the mouse */
	UPROPERTY(Category = Gameplay, EditAnywhere)
	float mouseY;

public:
	UFUNCTION()
	const float& GetMouseLocationX() const;

	UFUNCTION()
	const float& GetMouseLocationY() const;
};
