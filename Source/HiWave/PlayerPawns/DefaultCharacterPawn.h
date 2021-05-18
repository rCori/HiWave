// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerPawns/HiWavePawn.h"
#include "DefaultCharacterPawn.generated.h"

/**
 * 
 */
UCLASS()
class HIWAVE_API ADefaultCharacterPawn : public AHiWavePawn
{
	GENERATED_BODY()

protected:
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Capsule component for burst weapon radius */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* BurstComponent;

public:
	ADefaultCharacterPawn();

	void CharacterTick(float DeltaSeconds) override;

	virtual void DoBurstChild() override;

	/* The visual effects of getting hit */
	virtual void TakeHitVisuals() override;

	virtual void SetCharacterInvisible() override;

	virtual void DisabledInvincibleVisuals() override;

	virtual void EnabledInvincibleVisuals() override;

	/** Particle to emit when an enemy hits us */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* HitParticle;

	/** Sound to play each time enemy is hit */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* DeathSound;

	UPROPERTY(Category = Gameplay, BlueprintReadOnly)
	FVector burstComponentRelativeScale;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DefaultBodyMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkingBodyMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* DefaultWingMaterial;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkingWingMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

};
