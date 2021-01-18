// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "HiWavePawn.generated.h"


class AHiWaveGameState;
class AHiWavePlayerController;
class AItemPool;
class AHiWaveGameMode;

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(LogPlayerDeath, Log, All);

UCLASS(Blueprintable)
class AHiWavePawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


public:
	AHiWavePawn();

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	TArray<FVector> GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/** Sound to play each time enemy is hit */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* DeathSound;

	/** Sound to play when burst attack is used */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* BurstSound;

	/** Sound to play when current life starts */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* PlayerSpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class USphereComponent* SphereComponent;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	/** Particle to emit when we do the burst attack */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* BurstParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedBurstParticle;

	/** Capsule component for burst weapon radius */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BurstComponent;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	bool bIsDead;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DefaultBodyMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkingBodyMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DefaultWingMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkingWingMaterial;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void FireShot();

	/* Do the burst attack */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void DoBurst();
	
	/* Pause or unpause the game */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void PauseFunction();

	/* Restart the game if the player is dead*/
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void RestartFunction();

	UFUNCTION(BlueprintImplementableEvent)
	void ExpandBurstComponent();

	/* Overlap function for the burst collision */
	UFUNCTION()
	void OnBurstOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Call when hit by the enemy */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void TakeHit();

	/* After the burst attack reset the size of the capsule collision component */
	UFUNCTION(Category = Gameplay)
	void ResetBurstCollision();

	/* After the burst attack reset the size of the capsule collision component */
	UFUNCTION(Category = Gameplay)
	void ResetBurstAvailability();

	/* Directly incrase the amount of burst gauge */
	UFUNCTION(Category = Burst)
	void IncreaseBurst(float amount);

	/* Temporarily stop the multiplier value from decreasing */
	UFUNCTION(Category = Gameplay)
	void HaltMultiplierDecay();

	/* continue multiplier being decayed */
	UFUNCTION()
	void RestartMultiplierDecay();

	/* 
	 * Called on a Timer from TakeHit()
	 * All this does is get the game mode AHiWaveGameMode from the world and
	 * call DestroyAndRespawnPlayer() on it. Only exists as this function so
	 * it can be called on a timer
	 */
	UFUNCTION(Category = SpawnSystem, BlueprintCallable)
	void DoDeathAndRespawn() const;

	UFUNCTION(Category = SpawnSystem, BlueprintCallable)
	void SpawnInvincibility();
	
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void SetInvincible(const bool &isInvincible);

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void ChangeBulletLevel(const int &bulletLevel);

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName AimForwardBinding;
	static const FName AimRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	static const FName FireBinding;
	static const FName BurstBinding;
	static const FName PauseBinding;
	static const FName RestartBinding;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> fireRate;

	/* How long between player being hit and spawning again */
	UPROPERTY(Category = SpawnSystem, EditDefaultsOnly, BlueprintReadOnly)
	float spawnTimer;

	/* How fast the burst attack comes back */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float burstCollisionTimer;

	/* How fast the burst attack comes back */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float burstAvailabilityTimer;

	UPROPERTY(Category = Gameplay, BlueprintReadOnly)
	FVector burstComponentRelativeScale;

	/* The rate we accelerate in any direction */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float Acceleration;

	/* Maximum speed */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	/* Friction to constant apply to speed */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float Friction;

	UPROPERTY(Category = Gameplay, BlueprintReadOnly)
	FVector CurrentSpeed;

	/* Modified by DoBurst() and ResetBurstCollision(). Sets wether the burst attack can be done */
	UPROPERTY(Category = Burst, BlueprintReadOnly)
	bool bBurstAvailable;

	/* Number that represents current progress to refilling burst usage */
	UPROPERTY(Category = Burst, BlueprintReadOnly)
	float burstProgress;

	/* Burst progress must be at this level to set bBurstAvailable to true */
	UPROPERTY(Category = Burst, EditDefaultsOnly, BlueprintReadOnly)
	float maxBurst;

	/* Multiplier decay rate */
	UPROPERTY(Category = Burst, EditDefaultsOnly, BlueprintReadOnly)
	float multiplierDecayRate;

	/* Multiplier decay pause time */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float multiplierPauseTime;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCameraShake> PlayerDeathCameraShake;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float InvincibilityTimeLimit;

private:

	/* Only for internal use to point the player in the direction of the mouse */
	const FRotator RotateWithMouse();

	/* POint the cursor with the gamepad right thumbstick */
	const FRotator RotateWithGamepad();

	/* Bound to pressing and releasing the 'FireBinding' action*/
	void HoldFire();
	void ReleaseFire();

	/* Modified by HoldFire() and ReleaseFire() which are bound to 'FireBinding' action */
	bool bFireHeld;

	/* Keeps track of last shot fired. Once larger than fireRate player can shoot again. */
	float fireTimer;

	/* Is the game in pause state */
	bool bIsPaused;

	/* know if cursor is showing or not */
	bool bCursorIsShowing;

	/* Is the player invincible */
	bool bIsInvincible;

	/* current rate the multiplier is decaying at */
	float currentMultiplierDecayRate;

	AHiWaveGameState* hiWaveGameState;
	AHiWavePlayerController* pc;
	FTimerDelegate multiplierDecayResetDelegate;
	FTimerHandle multiplierDecayResetHandle;
	FRotator currentRotation;
	APlayerCameraManager* cameraManager;
	AHiWaveGameMode* hiWaveGameMode;
	AItemPool* bulletPool;

	FVector currentGunOffset;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

