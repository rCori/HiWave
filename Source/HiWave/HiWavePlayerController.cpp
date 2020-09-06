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
	const float prevMouseX = mouseX;
	const float prevMouseY = mouseY;
	GetMousePosition(mouseX, mouseY);
	APlayerController::Tick(DeltaTime);

	bMouseIsMoving = !FMath::IsNearlyZero(prevMouseX - mouseX) || !FMath::IsNearlyZero(prevMouseY - mouseY);
}

void AHiWavePlayerController::BeginPlay() {
	bShowMouseCursor = true;
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);
}

const float& AHiWavePlayerController::GetMouseLocationX() const {
	return mouseX;
}

const float& AHiWavePlayerController::GetMouseLocationY() const{
	return mouseY;
}

const bool& AHiWavePlayerController::GetIsMouseMoving() const {
	return bMouseIsMoving;
}