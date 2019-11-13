// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWavePawn.h"
#include "HiWaveProjectile.h"
#include "HiWavePlayerController.h"
#include "HiWaveGameMode.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

const FName AHiWavePawn::MoveForwardBinding("MoveForward");
const FName AHiWavePawn::MoveRightBinding("MoveRight");
const FName AHiWavePawn::FireForwardBinding("FireForward");
const FName AHiWavePawn::FireRightBinding("FireRight");
const FName AHiWavePawn::FireBinding("Fire");

AHiWavePawn::AHiWavePawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	//ShipMeshComponent->BodyInstance.SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Cache our hit particle
	//HitParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC"));
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> HP(TEXT("ParticleSystem'/Game/Particles/PlayerHitParticle.PlayerHitParticle'"));
	//HitParticle->SetTemplate(HP.Object);
	//HitParticle->SetupAttachment(RootComponent);
	//HitParticle->bAutoActivate = false;


	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bFireHeld = false;
	bIsDead = false;
}

void AHiWavePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAction(FireBinding, IE_Pressed, this, &AHiWavePawn::HoldFire);
	PlayerInputComponent->BindAction(FireBinding, IE_Released, this, &AHiWavePawn::ReleaseFire);
}

void AHiWavePawn::Tick(float DeltaSeconds)
{
	//Can't fire if you are dead
	if (bIsDead) return;
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	
	FRotator NewRotation = RotateWithMouse();
	RootComponent->SetRelativeRotation(NewRotation);

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		//const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		
		//RotateWithMouse();

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}
	//else {
		
	//
	

	// Try and fire a shot
	if (bFireHeld) {
		FireShot();
	}	
}

void AHiWavePawn::FireShot()
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		const FRotator FireRotation = GetActorRotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile
			World->SpawnActor<AHiWaveProjectile>(SpawnLocation, FireRotation);
		}

		bCanFire = false;
		World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AHiWavePawn::ShotTimerExpired, FireRate);

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		bCanFire = false;
	}
}

void AHiWavePawn::ShotTimerExpired()
{
	bCanFire = true;
}

const FRotator AHiWavePawn::RotateWithMouse() {
	AHiWavePlayerController* pc = Cast<AHiWavePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	FRotator newRotator;
	if (pc != nullptr) {
		FVector actorLocation = GetActorLocation();
		const float &mouseX = pc->GetMouseLocationX();
		const float &mouseY = pc->GetMouseLocationY();
		FVector mouseWorldDirection;
		FVector mouseWorldLocation;
		pc->DeprojectScreenPositionToWorld(mouseX, mouseY, mouseWorldLocation, mouseWorldDirection);
		//Now we have to project into the world
		// get the camera transform
		FVector CameraLoc;
		FRotator CameraRot;
		GetActorEyesViewPoint(CameraLoc, CameraRot);

		FHitResult Hit(ForceInit);
		//FVector start = mouseWorldLocation;
		FVector End = mouseWorldLocation + (mouseWorldDirection*2200.0f);
		FCollisionQueryParams CollisionParams;
		FVector Start = mouseWorldLocation;

		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2.f, false, 4.f);

		//GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, CollisionParams);
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams);

		FVector tempLookAtTarget;
		if (Hit.bBlockingHit)
		{
			tempLookAtTarget = Hit.Location;
		}
		tempLookAtTarget.Z = actorLocation.Z;
		newRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), tempLookAtTarget);
		
	}
	else {
		newRotator = FRotator();
	}
	return newRotator;
}

void AHiWavePawn::HoldFire() {
	bFireHeld = true;
}

void AHiWavePawn::ReleaseFire() {
	bFireHeld = false;
}

void AHiWavePawn::TakeHit() {

	//Turn off collision
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshComponent->SetVisibility(false);

	FTimerDelegate TimerDeathAndRespawn;
	FTimerHandle TimerHandleDeathAndRespawn;
	TimerDeathAndRespawn.BindUFunction(this, FName("DoDeathAndRespawn"));

	if (HitParticle != nullptr) {
		//UE_LOG(LogTemp, Warning, TEXT("Player is hit going to spawn %s"), *HitParticle->GetFName().ToString());
		FRotator rotation = FRotator::ZeroRotator;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), rotation);
	}

	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleDeathAndRespawn, TimerDeathAndRespawn, 2.f, false);
}

void AHiWavePawn::DoDeathAndRespawn() {
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAndRespawnPlayer();
}