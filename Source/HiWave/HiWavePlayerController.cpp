// Fill out your copyright notice in the Description page of Project Settings.


#include "HiWavePlayerController.h"

//Constructor
AHiWavePlayerController::AHiWavePlayerController()
	:
	APlayerController()
{
	//... custom defaults ...
}

//... custom code ...

void AHiWavePlayerController::Tick(float DeltaTime) {
	GetMousePosition(mouseX, mouseY);
	APlayerController::Tick(DeltaTime);
}

void AHiWavePlayerController::BeginPlay() {
	bShowMouseCursor = true;
}

const float& AHiWavePlayerController::GetMouseLocationX() const {
	return mouseX;
}

const float& AHiWavePlayerController::GetMouseLocationY() const{
	return mouseY;
}