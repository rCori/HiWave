// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "HiWavePawn.generated.h"


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
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	bool bIsDead;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void FireShot();

	/* Call when hit by the enemy */
	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void TakeHit();

	/* 
	 * Called on a Timer from TakeHit()
	 * All this does is get the game mode AHiWaveGameMode from the world and
	 * call DestroyAndRespawnPlayer() on it. Only exists as this function so
	 * it can be called on a timer
	 */
	UFUNCTION(Category = SpawnSystem, BlueprintCallable)
	void DoDeathAndRespawn() const;
	

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	static const FName FireBinding;

protected:
	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float moveSpeed;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadOnly)
	float fireRate;

	/* How long between player being hit and spawning again */
	UPROPERTY(Category = SpawnSystem, EditDefaultsOnly, BlueprintReadOnly)
	float spawnTimer;

private:

	/* Only for internal use to point the player in the direction of the mouse */
	const FRotator RotateWithMouse();

	/* Bound to pressing and releasing the 'FireBinding' action*/
	void HoldFire();
	void ReleaseFire();

	/* Modified by HoldFire() and ReleaseFire() which are bound to 'FireBinding' action */
	bool bFireHeld;

	/* Keeps track of last shot fired. Once larger than fireRate player can shoot again. */
	float fireTimer;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

