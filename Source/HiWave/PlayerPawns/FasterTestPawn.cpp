// Fill out your copyright notice in the Description page of Project Settings.


#include "FasterTestPawn.h"
#include "HiWaveGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

AFasterTestPawn::AFasterTestPawn()
{
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
	BurstComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BurstHitDetection"));
	//BurstComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	//BurstComponent->SetupAttachment(RootComponent);
	BurstComponent->SetupAttachment(ShipMeshComponent);
	//BurstComponent->ComponentTags.Add("BurstHitbox");

	BurstSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BurstSphereComponent"));
	BurstSphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	BurstSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	BurstSphereComponent->SetupAttachment(BurstComponent);
	BurstSphereComponent->ComponentTags.Add("BurstHitbox");
}

void AFasterTestPawn::BeginPlay()
{

	burstComponentRelativeScale = BurstComponent->GetComponentScale();
	BurstComponent->SetWorldScale3D(FVector::ZeroVector);

	ShipMeshComponent->SetMaterial(0, DefaultBodyMaterial);
	ShipMeshComponent->SetMaterial(1, DefaultWingMaterial);

	Super::BeginPlay();
}

void AFasterTestPawn::CharacterTick(float DeltaSeconds)
{
	hiWaveGameState->IncreaseMultiplier(currentMultiplierDecayRate*DeltaSeconds);
}

void AFasterTestPawn::DoBurstChild()
{
	//Play burst sound
	if (BurstSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, BurstSound, GetActorLocation());
	}
}


void AFasterTestPawn::TakeHitVisuals()
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


void AFasterTestPawn::SetCharacterInvisible()
{
	ShipMeshComponent->SetVisibility(false);
}

void AFasterTestPawn::DisabledInvincibleVisuals()
{
}

void AFasterTestPawn::EnabledInvincibleVisuals()
{
}