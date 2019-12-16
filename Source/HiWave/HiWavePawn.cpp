// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWavePawn.h"
#include "HiWaveProjectile.h"
#include "HiWavePlayerController.h"
#include "GameModes/HiWaveGameMode.h"

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
#include "Components/CapsuleComponent.h"
#include "EnemyPawns/EnemyPawn.h"

const FName AHiWavePawn::MoveForwardBinding("MoveForward");
const FName AHiWavePawn::MoveRightBinding("MoveRight");
const FName AHiWavePawn::FireForwardBinding("FireForward");
const FName AHiWavePawn::FireRightBinding("FireRight");
const FName AHiWavePawn::FireBinding("Fire");
const FName AHiWavePawn::BurstBinding("Burst");

DEFINE_LOG_CATEGORY(LogPlayerDeath);

AHiWavePawn::AHiWavePawn()
{	
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ShipMeshComponent->ComponentTags.Add("ShipMesh");

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

	//Create hitbox for burst capsule
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BurstHitDetection"));
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	CapsuleComponent->SetupAttachment(RootComponent);
	CapsuleComponent->ComponentTags.Add("BurstHitbox");

	// Movement
	moveSpeed = 1000.0f;
	speedRatio = 0.75;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	fireRate = 0.1f;
	fireTimer = 0.0f;
	bFireHeld = false;
	bIsDead = false;
	spawnTimer = 2.0f;
}

void AHiWavePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAction(FireBinding, IE_Pressed, this, &AHiWavePawn::HoldFire);
	PlayerInputComponent->BindAction(FireBinding, IE_Released, this, &AHiWavePawn::ReleaseFire);
	PlayerInputComponent->BindAction(BurstBinding, IE_Released, this, &AHiWavePawn::DoBurst);
}

void AHiWavePawn::Tick(float DeltaSeconds)
{
	//Can't fire if you are dead
	if (bIsDead) return;
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.0f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	FVector Movement = MoveDirection * (moveSpeed)* speedRatio * DeltaSeconds;
	
	FRotator NewRotation = RotateWithMouse();
	RootComponent->SetRelativeRotation(NewRotation);

	if (bFireHeld) {
		Movement -= NewRotation.Vector() * (moveSpeed) * (1- speedRatio) * DeltaSeconds;
	}

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

	//Update timer to when we can fire again
	if (fireTimer < fireRate) {
		fireTimer += DeltaSeconds;
	}

	// Try and fire a shot
	if (bFireHeld) {
		FireShot();
	}	
}

void AHiWavePawn::FireShot()
{
	// If it's ok to fire again
	if (fireTimer>= fireRate)
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

		//Reset timer after successful fire
		fireTimer = 0.0f;

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void AHiWavePawn::DoBurst()
{
	UE_LOG(LogTemp, Warning, TEXT("Did burst attack!"));
	TArray<AActor*> overlappingActors;
	CapsuleComponent->SetCapsuleSize(capsuleRadius, capsuleHalfHeight, true);

	//Create a timer to call DoDeathAndRespawn in some number of seconds
	FTimerDelegate TimerResetBurstCollision;
	FTimerHandle TimerHandleResetBurstCollision;
	TimerResetBurstCollision.BindUFunction(this, FName("ResetBurstCollision"));

	GetWorld()->GetTimerManager().SetTimer(TimerHandleResetBurstCollision, TimerResetBurstCollision, 0.4f, false);
}

void AHiWavePawn::OnBurstOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(OtherActor);
	if (enemyPawn != NULL) {
		UE_LOG(LogPlayerDeath, Warning, TEXT("[AHiWavePawn with enemy pawn %s] OnBurstOverlap"), *(enemyPawn->GetActorLabel()));
		//enemyPawn->TakeHit();
	}
}


void AHiWavePawn::HoldFire() {
	bFireHeld = true;
}

void AHiWavePawn::ReleaseFire() {
	bFireHeld = false;
}

void AHiWavePawn::TakeHit() {

	UE_LOG(LogPlayerDeath, Warning, TEXT("[HiWavePawn] TakeHit"));

	//Turn off collision
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshComponent->SetVisibility(false);

	//Create a timer to call DoDeathAndRespawn in some number of seconds
	FTimerDelegate TimerDeathAndRespawn;
	FTimerHandle TimerHandleDeathAndRespawn;
	TimerDeathAndRespawn.BindUFunction(this, FName("DoDeathAndRespawn"));

	//Spawn the death particle
	if (HitParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), rotation);
	}

	//Destroy all the enemies and get ready to go through player respawn process.
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAllEnemies();
	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleDeathAndRespawn, TimerDeathAndRespawn, 2.f, false);
}

void AHiWavePawn::ResetBurstCollision()
{
	CapsuleComponent->SetCapsuleSize(0, 0, false);
}

void AHiWavePawn::DoDeathAndRespawn() const {
	UE_LOG(LogPlayerDeath, Warning, TEXT("[HiWavePawn] DoDeathAndRespawn"));
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAndRespawnPlayer();
}

void AHiWavePawn::BeginPlay()
{	
	capsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	capsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	CapsuleComponent->SetCapsuleSize(0, 0, false);
	
	Super::BeginPlay();
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
		FVector End = mouseWorldLocation + (mouseWorldDirection*2800.0f);
		FCollisionQueryParams CollisionParams;
		FVector Start = mouseWorldLocation;

		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 2.f, false, 4.f);

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