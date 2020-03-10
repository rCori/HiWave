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
//#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "EnemyPawns/EnemyPawn.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

const FName AHiWavePawn::MoveForwardBinding("MoveForward");
const FName AHiWavePawn::MoveRightBinding("MoveRight");
const FName AHiWavePawn::FireForwardBinding("FireForward");
const FName AHiWavePawn::FireRightBinding("FireRight");
const FName AHiWavePawn::FireBinding("Fire");
const FName AHiWavePawn::BurstBinding("Burst");
const FName AHiWavePawn::PauseBinding("Pause");

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
	BurstComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurstHitDetection"));
	BurstComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	BurstComponent->SetupAttachment(RootComponent);
	BurstComponent->ComponentTags.Add("BurstHitbox");

	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	fireRate = 0.1f;
	fireTimer = 0.0f;
	burstCollisionTimer = 0.4f;
	burstAvailabilityTimer = 1.5f;
	bFireHeld = false;
	bIsDead = false;
	bBurstAvailable = true;
	spawnTimer = 2.0f;
	CurrentSpeed = FVector::ZeroVector;
	burstProgress = 20.0f;
	maxBurst = 20.0f;

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
	PlayerInputComponent->BindAction(PauseBinding, IE_Pressed, this, &AHiWavePawn::PauseFunction);

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

	//Apply constant acceleration to your speed
	CurrentSpeed += MoveDirection * Acceleration * DeltaSeconds;

	//Constantly subtract friction from your speed
	CurrentSpeed -= CurrentSpeed * Friction * DeltaSeconds;

	//Limit your speed to a maximum speed;
	if (FMath::Abs(CurrentSpeed.Size()) > MaxSpeed) {
		CurrentSpeed = MoveDirection * MaxSpeed * DeltaSeconds;
	}

	FRotator NewRotation = RotateWithMouse();
	RootComponent->SetRelativeRotation(NewRotation);

	// If non-zero size, move this actor
	if (CurrentSpeed.SizeSquared() > 0.0f)
	{
		//const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(CurrentSpeed*DeltaSeconds, NewRotation, true, &Hit);
		
		//RotateWithMouse();

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(CurrentSpeed, Normal2D) * (1.f - Hit.Time);
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
	if (!bBurstAvailable)
	{
		burstProgress += DeltaSeconds;
		if (burstProgress >= maxBurst) {
			burstProgress = maxBurst;
			bBurstAvailable = true;
		}
	}
}

void AHiWavePawn::FireShot()
{
	// If it's ok to fire again
	if (fireTimer>= fireRate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("fireTime at time of shot: %f"), fireTimer));

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
	if (bBurstAvailable && !bIsDead) {
		burstProgress = 0.0f;
		bBurstAvailable = false;
		UE_LOG(LogTemp, Warning, TEXT("Did burst attack!"));
		//CapsuleComponent->SetCapsuleSize(capsuleRadius, capsuleHalfHeight, true);
		//BoxComponent->SetBoxExtent(boxExtent, true);
		//BurstComponent->SetWorldScale3D(burstComponentRelativeScale);
		ExpandBurstComponent();

		//Spawn the death particle
		if (BurstParticle != nullptr) {
			FRotator rotation = GetActorRotation();
			spawnedBurstParticle = UGameplayStatics::SpawnEmitterAttached(BurstParticle,RootComponent,NAME_None, FVector::ZeroVector, rotation, EAttachLocation::KeepRelativeOffset);
			//spawnedBurstParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BurstParticle, GetActorLocation(), rotation);
			spawnedBurstParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		//FTimerDelegate TimerResetBurstCollision;
		//FTimerHandle TimerHandleResetBurstCollision;
		//TimerResetBurstCollision.BindUFunction(this, FName("ResetBurstCollision"));

		//GetWorld()->GetTimerManager().SetTimer(TimerHandleResetBurstCollision, TimerResetBurstCollision, burstCollisionTimer, false);


		//FTimerDelegate TimerResetBurstAvailability;
		//FTimerHandle TimerHandleResetBurstAvailability;
		//TimerResetBurstAvailability.BindUFunction(this, FName("ResetBurstAvailability"));

		//GetWorld()->GetTimerManager().SetTimer(TimerHandleResetBurstAvailability, TimerResetBurstAvailability, burstAvailabilityTimer, false);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Burst attack not available"));
	}
}

void AHiWavePawn::PauseFunction() {
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->OpenPauseMenu();
}

void AHiWavePawn::OnBurstOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(OtherActor);
	if (enemyPawn != NULL) {
		//UE_LOG(LogPlayerDeath, Warning, TEXT("[AHiWavePawn with enemy pawn %s] OnBurstOverlap"), *(enemyPawn->GetActorLabel()));
		//enemyPawn->TakeHit();
		enemyPawn->BurstOverlap();
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
	//CapsuleComponent->SetCapsuleSize(0, 0, false);
	//BoxComponent->SetBoxExtent(FVector::ZeroVector, false);
	BurstComponent->SetRelativeScale3D(FVector::ZeroVector);
}

void AHiWavePawn::ResetBurstAvailability()
{
	bBurstAvailable = true;
}

void AHiWavePawn::IncreaseBurst(float amount)
{
	burstProgress += amount;
}

void AHiWavePawn::DoDeathAndRespawn() const {
	UE_LOG(LogPlayerDeath, Warning, TEXT("[HiWavePawn] DoDeathAndRespawn"));
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAndRespawnPlayer();
}

void AHiWavePawn::BeginPlay()
{	
	//capsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	//capsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	//CapsuleComponent->SetCapsuleSize(0, 0, false);
	
	//boxExtent = BoxComponent->GetScaledBoxExtent();
	//BoxComponent->SetBoxExtent(FVector::ZeroVector, false);
	burstComponentRelativeScale = BurstComponent->GetComponentScale();
	BurstComponent->SetWorldScale3D(FVector::ZeroVector);


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