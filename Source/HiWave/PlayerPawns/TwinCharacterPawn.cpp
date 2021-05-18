// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinCharacterPawn.h"
#include "HiWaveGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

ATwinCharacterPawn::ATwinCharacterPawn() {
	LeftHitboxCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftCapsuleHitbox"));
	LeftHitboxCollision->SetupAttachment(RootComponent);

	RightHitboxCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightCapsuleHitbox"));
	RightHitboxCollision->SetupAttachment(RootComponent);

	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshComponent->SetupAttachment(RootComponent);

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(ShipMeshComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	//Create hitbox for burst capsule
	BurstComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurstHitDetection"));
	BurstComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	BurstComponent->SetupAttachment(ShipMeshComponent);
	BurstComponent->ComponentTags.Add("BurstHitbox");
}

void ATwinCharacterPawn::BeginPlay()
{
	Super::BeginPlay();
	isLeftOffset = true;
}


void ATwinCharacterPawn::CharacterTick(float DeltaSeconds)
{
	hiWaveGameState->IncreaseMultiplier(currentMultiplierDecayRate*DeltaSeconds);
}

void ATwinCharacterPawn::DoBurstChild()
{
}


void ATwinCharacterPawn::FireShotChild()
{
	// Everytime you shoot the offset moves from either the left to right or right to left offset.
	isLeftOffset = !isLeftOffset;
	if (isLeftOffset) {
		currentGunOffset = GunOffset[bulletLevel] + FVector(0, leftGunOffset.Y, 0);
	}
	else {
		currentGunOffset = GunOffset[bulletLevel] + FVector(0, rightGunOffset.Y, 0);
	}
}

void ATwinCharacterPawn::TakeHitVisuals()
{
}

void ATwinCharacterPawn::SetCharacterInvisible()
{
	ShipMeshComponent->SetVisibility(false);
}

void ATwinCharacterPawn::DisabledInvincibleVisuals()
{
}

void ATwinCharacterPawn::EnabledInvincibleVisuals()
{
}

