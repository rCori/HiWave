// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacterPawn.h"
#include "HiWaveGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ADefaultCharacterPawn::ADefaultCharacterPawn() {
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

void ADefaultCharacterPawn::BeginPlay()
{

	burstComponentRelativeScale = BurstComponent->GetComponentScale();
	BurstComponent->SetWorldScale3D(FVector::ZeroVector);

	Super::BeginPlay();
}

void ADefaultCharacterPawn::CharacterTick(float DeltaSeconds)
{
	hiWaveGameState->IncreaseMultiplier(currentMultiplierDecayRate*DeltaSeconds);
}


void ADefaultCharacterPawn::DoBurstChild()
{
	//Spawn the death particle
	if (BurstParticle != nullptr) {
		FRotator rotation = GetActorRotation();
		spawnedBurstParticle = UGameplayStatics::SpawnEmitterAttached(BurstParticle, RootComponent, NAME_None, FVector::ZeroVector, rotation, EAttachLocation::KeepRelativeOffset);
		spawnedBurstParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//Play burst sound
	if (BurstSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, BurstSound, GetActorLocation());
	}
}


void ADefaultCharacterPawn::TakeHitVisuals()
{
	//Spawn the death particle
	if (HitParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), rotation);
	}

	if (DeathSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

void ADefaultCharacterPawn::SetCharacterInvisible()
{
	ShipMeshComponent->SetVisibility(false);
}

void ADefaultCharacterPawn::DisabledInvincibleVisuals()
{
	ShipMeshComponent->SetMaterial(0, DefaultBodyMaterial);
	ShipMeshComponent->SetMaterial(1, DefaultWingMaterial);
}

void ADefaultCharacterPawn::EnabledInvincibleVisuals()
{
	ShipMeshComponent->SetMaterial(0, BlinkingBodyMaterial);
	ShipMeshComponent->SetMaterial(1, BlinkingWingMaterial);
}

void ADefaultCharacterPawn::ResetBurstCollision()
{
	BurstComponent->SetRelativeScale3D(FVector::ZeroVector);
}