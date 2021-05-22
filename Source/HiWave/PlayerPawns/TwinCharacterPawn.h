// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerPawns/HiWavePawn.h"
#include "TwinCharacterPawn.generated.h"


/**
 * 
 */
UCLASS()
class HIWAVE_API ATwinCharacterPawn : public AHiWavePawn
{
	GENERATED_BODY()
	
protected:

	/* Going to need an extra hitboxes because of the shape of the thing*/
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* LeftHitboxCollision;

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* RightHitboxCollision;

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
	ATwinCharacterPawn();

	void CharacterTick(float DeltaSeconds) override;

	virtual void DoBurstChild() override;

	virtual void FireShotChild() override;

	/* The visual effects of getting hit */
	virtual void TakeHitVisuals() override;

	virtual void SetCharacterInvisible() override;

	virtual void DisabledInvincibleVisuals() override;

	virtual void EnabledInvincibleVisuals() override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", Meta = (MakeEditWidget = true))
	FVector leftGunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", Meta = (MakeEditWidget = true))
	FVector rightGunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FVector burstComponentRelativeScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	UMaterialInstanceDynamic *dynamicDiamondMaterial;

private:
	bool isLeftOffset;
};
