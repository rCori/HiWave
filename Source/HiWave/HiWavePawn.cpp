// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWavePawn.h"
#include "HiWaveProjectile.h"
#include "HiWavePlayerController.h"
#include "GameModes/HiWaveGameMode.h"
#include "PoolableActor.h"
#include "PoolableTypes.h"
#include "ItemPool.h"
#include "TutorialCountTypes.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/PlayerCameraManager.h"
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
#include "Components/BoxComponent.h"
#include "EnemyPawns/EnemyPawn.h"
#include "Components/SphereComponent.h"
#include "HiWaveGameState.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

const FName AHiWavePawn::MoveForwardBinding("MoveForward");
const FName AHiWavePawn::MoveRightBinding("MoveRight");
const FName AHiWavePawn::FireForwardBinding("FireForward");
const FName AHiWavePawn::FireRightBinding("FireRight");
const FName AHiWavePawn::FireBinding("Fire");
const FName AHiWavePawn::BurstBinding("Burst");
const FName AHiWavePawn::PauseBinding("Pause");
const FName AHiWavePawn::RestartBinding("Restart");

//Only used for gamepad controls
const FName AHiWavePawn::AimForwardBinding("AimForward");
const FName AHiWavePawn::AimRightBinding("AimRight");

DEFINE_LOG_CATEGORY(LogPlayerDeath);

AHiWavePawn::AHiWavePawn()
{	

	//Create hitsphere
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	/*
	// Create the mesh component
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
	*/

	//Create hitbox for burst capsule
	//BurstComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurstHitDetection"));
	//BurstComponent->OnComponentBeginOverlap.AddDynamic(this, &AHiWavePawn::OnBurstOverlap);
	//BurstComponent->SetupAttachment(ShipMeshComponent);
	//BurstComponent->ComponentTags.Add("BurstHitbox");

	// Weapon
	GunOffset = { FVector(110.f, 0.f, 0.f), FVector(150.f, 0.f, 0.f), FVector(150.f, 0.f, 0.f) };
	fireRate = { 0.1f, 0.08, 0.06 };
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
	multiplierDecayRates = { -0.80, -0.80, -0.80 };
	multiplierPauseTimes = { 0.1, 0.1, 0.1 };
	bCursorIsShowing = true;
	currentRotation = GetActorRotation();
	burstTimer = 0.0;
}

void AHiWavePawn::BeginPlay()
{
	//burstComponentRelativeScale = BurstComponent->GetComponentScale();
	//BurstComponent->SetWorldScale3D(FVector::ZeroVector);
	pc = Cast<AHiWavePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

	//Play spawning sound
	if (PlayerSpawnSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, PlayerSpawnSound, GetActorLocation());
	}
	currentGunOffset = GunOffset[0];
	currentMultiplierDecayRate = multiplierDecayRates[0];
	SpawnInvincibility();

	Super::BeginPlay();
}

void AHiWavePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(AimForwardBinding);
	PlayerInputComponent->BindAxis(AimRightBinding);
	PlayerInputComponent->BindAction(FireBinding, IE_Pressed, this, &AHiWavePawn::HoldFire);
	PlayerInputComponent->BindAction(FireBinding, IE_Released, this, &AHiWavePawn::ReleaseFire);
	PlayerInputComponent->BindAction(BurstBinding, IE_Released, this, &AHiWavePawn::DoBurst);
	PlayerInputComponent->BindAction(PauseBinding, IE_Pressed, this, &AHiWavePawn::PauseFunction);
	PlayerInputComponent->BindAction(RestartBinding, IE_Pressed, this, &AHiWavePawn::RestartFunction);

}

void AHiWavePawn::Tick(float DeltaSeconds)
{
	//Can't fire if you are dead
	if (bIsDead) return;

	

	if (hiWaveGameState == nullptr) {
		hiWaveGameState = Cast<AHiWaveGameState>(GetWorld()->GetGameState());
		hiWaveGameState->OnMultiplierLevelChanged.AddDynamic(this, &AHiWavePawn::ChangeBulletLevel);
	}

	if (hiWaveGameMode == nullptr) {
		hiWaveGameMode = Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode());
	}

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

	//Test to see if the mouse is moving
	float mouseMoveX, mouseMoveY;
	pc->GetInputMouseDelta(mouseMoveX, mouseMoveY);

	float thumbstickForward = GetInputAxisValue(AimForwardBinding);
	float thumbstickRight = GetInputAxisValue(AimRightBinding);

	bool movingThumbstick = !(FMath::IsNearlyZero(GetInputAxisValue(AimForwardBinding)) && FMath::IsNearlyZero(GetInputAxisValue(AimRightBinding)));

	//FRotator NewRotation;
	if (movingThumbstick) {
		currentRotation = RotateWithGamepad();
		if (bCursorIsShowing) {
			bCursorIsShowing = false;
			pc->bShowMouseCursor = false;
			FInputModeGameOnly inputMode;
			inputMode.SetConsumeCaptureMouseDown(true);
			pc->SetInputMode(inputMode);
		}
	} else if(pc->GetIsMouseMoving()) {
		currentRotation = RotateWithMouse();
		if (!bCursorIsShowing) {
			bCursorIsShowing = true;
			pc->bShowMouseCursor = true;
			FInputModeGameOnly inputMode;
			inputMode.SetConsumeCaptureMouseDown(false);
			pc->SetInputMode(inputMode);
		}
	}
	RootComponent->SetRelativeRotation(currentRotation);

	// If non-zero size, move this actor
	if (CurrentSpeed.SizeSquared() > 0.0f)
	{
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(CurrentSpeed*DeltaSeconds, currentRotation, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(CurrentSpeed, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection*DeltaSeconds, currentRotation, true);
		}
	}

	//Update timer to when we can fire again
	if (fireTimer < fireRate[hiWaveGameState->GetMultiplierIndex()]) {
		fireTimer += DeltaSeconds;
	}

	// Try and fire a shot
	if (bFireHeld) {
		FireShot();
	}
	if (!bBurstAvailable)
	{
		burstProgress += DeltaSeconds;
		burstTimer += DeltaSeconds;
		if (burstProgress >= maxBurst) {
			burstProgress = maxBurst;
			bBurstAvailable = true;
			if (burstTimer < maxBurst * hiWaveGameState->burstGoalPercent) {
				hiWaveGameState->UpdateTutorialValue(ETutorialCountTypes::VE_BurstRecharge);
			}
			burstTimer = 0.0;
		}
	}

	

	CharacterTick(DeltaSeconds);

}

void AHiWavePawn::CharacterTick(float DeltaSeconds)
{
}

void AHiWavePawn::FireShot()
{
	// If it's ok to fire again
	if (fireTimer>= fireRate[hiWaveGameState->GetMultiplierIndex()])
	{
		const FRotator FireRotation = GetActorRotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(currentGunOffset);
		
		if (bulletPool == nullptr) {
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPool::StaticClass(), FoundActors);
			bulletPool = Cast<AItemPool>(FoundActors[0]);
		}

		AHiWaveProjectile* bullet = Cast<AHiWaveProjectile>(bulletPool->GetPooledObject(EPoolableType::VE_PlayerBullet));
		if (bullet != nullptr) {
			IPoolableObjectInterface::Execute_SetActive(bullet, true);
			bullet->SetLocationAndRotation(SpawnLocation, FireRotation);
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
		ExpandBurstComponent();

		DoBurstChild();

		/*
		//Spawn the death particle
		if (BurstParticle != nullptr) {
			FRotator rotation = GetActorRotation();
			spawnedBurstParticle = UGameplayStatics::SpawnEmitterAttached(BurstParticle,RootComponent,NAME_None, FVector::ZeroVector, rotation, EAttachLocation::KeepRelativeOffset);
			spawnedBurstParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		//Play burst sound
		if (BurstSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(this, BurstSound, GetActorLocation());
		}
		*/
	}
}

void AHiWavePawn::DoBurstChild()
{

}

void AHiWavePawn::PauseFunction() {
	hiWaveGameMode->TogglePause();
	hiWaveGameMode->OpenPauseMenu();
}

void AHiWavePawn::RestartFunction() {
	hiWaveGameMode->RestartGame();
}

void AHiWavePawn::OnBurstOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(OtherActor);
	if (enemyPawn != NULL) {
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

	if (bIsInvincible) {
		return;
	}

	//Turn off collision
	RemoveCollisionMakeInvisible();

	//Create a timer to call DoDeathAndRespawn in some number of seconds
	FTimerDelegate TimerDeathAndRespawn;
	FTimerHandle TimerHandleDeathAndRespawn;
	TimerDeathAndRespawn.BindUFunction(this, FName("DoDeathAndRespawn"));

	TakeHitVisuals();

	//Destroy all the enemies and get ready to go through player respawn process.
	Cast<AHiWaveGameMode>(GetWorld()->GetAuthGameMode())->DestroyAllEnemies();
	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleDeathAndRespawn, TimerDeathAndRespawn, 2.f, false);

	//Play player death camera shake
	cameraManager->PlayCameraShake(PlayerDeathCameraShake, 1.0f);
}


void AHiWavePawn::TakeHitVisuals()
{
}


/*
void AHiWavePawn::ResetBurstCollision()
{	
	BurstComponent->SetRelativeScale3D(FVector::ZeroVector);
}
*/

void AHiWavePawn::ResetBurstAvailability()
{
	bBurstAvailable = true;
}

void AHiWavePawn::IncreaseBurst(float amount)
{
	burstProgress += amount;
}


void AHiWavePawn::ResetMultiplierDecayRate()
{
	currentMultiplierDecayRate = multiplierDecayRates[bulletLevel];
}

void AHiWavePawn::HaltMultiplierDecay()
{

	if (multiplierDecayResetHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(multiplierDecayResetHandle);
	}
	//If the decay pause time is 0 or less, then don't pause it.
	if (multiplierPauseTimes[bulletLevel] > 0.0) {
		currentMultiplierDecayRate = 0.0;
		//Clear the old timer
		//Start a timer to call RestartMultiplierDecay
		multiplierDecayResetDelegate.BindUFunction(this, FName("ResetMultiplierDecayRate"));
		GetWorld()->GetTimerManager().SetTimer(multiplierDecayResetHandle, multiplierDecayResetDelegate, multiplierPauseTimes[bulletLevel], false);
	}
	else {
		ResetMultiplierDecayRate();
	}
}

void AHiWavePawn::DoDeathAndRespawn() const {
	hiWaveGameMode->DestroyAndRespawnPlayer();
}


void AHiWavePawn::SpawnInvincibility() {
	EnabledInvincibleVisuals();
	bIsInvincible = true;
	FTimerDelegate SetInvincibleTimerDelegate;
	FTimerHandle SetInvincibleTimerHandle;
	SetInvincibleTimerDelegate.BindUFunction(this, FName("SetInvincible"),false);
	GetWorld()->GetTimerManager().SetTimer(SetInvincibleTimerHandle, SetInvincibleTimerDelegate, InvincibilityTimeLimit, false);
}

void AHiWavePawn::ChangeBulletLevel(const int &newBulletLevel)
{
	currentGunOffset = GunOffset[newBulletLevel];
	//multiplier dday rate is kept track of differently because it can be stopped and started in a way that currentGunOffset is not.
	this->bulletLevel = newBulletLevel;
	if (currentMultiplierDecayRate != 0.0) {
		currentMultiplierDecayRate = multiplierDecayRates[newBulletLevel];
	}
}

void AHiWavePawn::RemoveCollisionMakeInvisible()
{
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCharacterInvisible();
}

void AHiWavePawn::SetCharacterInvisible()
{
	
}

void AHiWavePawn::SetInvincible(const bool &isInvincible) {
	bIsInvincible = isInvincible;
	if (!isInvincible) {
		/*
		ShipMeshComponent->SetMaterial(0, DefaultBodyMaterial);
		ShipMeshComponent->SetMaterial(1, DefaultWingMaterial);
		*/
		DisabledInvincibleVisuals();
	}
}

void AHiWavePawn::DisabledInvincibleVisuals()
{

}

void AHiWavePawn::EnabledInvincibleVisuals()
{
}

const FRotator AHiWavePawn::RotateWithMouse() {
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
		FVector End = mouseWorldLocation + (mouseWorldDirection*3400.0f);
		FCollisionQueryParams CollisionParams;
		FVector Start = mouseWorldLocation;

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


const FRotator AHiWavePawn::RotateWithGamepad() {
	FRotator newRotator;

	const float ForwardValue = GetInputAxisValue(AimForwardBinding);
	const float RightValue = GetInputAxisValue(AimRightBinding);

	if (ForwardValue != 0.0f || RightValue != 0.0f) {
		FVector newRotationVector = FVector(-ForwardValue, RightValue, 0.0f);
		newRotator = newRotationVector.Rotation();
	}

	return newRotator;
}