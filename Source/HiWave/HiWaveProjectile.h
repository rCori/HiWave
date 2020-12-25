// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableObjectInterface.h"
#include "HiWaveProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UParticleSystemComponent;

UCLASS(config = Game)
class AHiWaveProjectile : public AActor, public IPoolableObjectInterface
{
	GENERATED_BODY()

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AHiWaveProjectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnStop(const FHitResult& Hit);

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	/** Sphere collision component */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	//UStaticMeshComponent* ProjectileMesh;

	//The mesh to be used at level 1
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MeshAssets)
	UStaticMesh* Level1Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MeshAssets)
	UStaticMesh* Level2Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MeshAssets)
	UStaticMesh* Level3Mesh;

	/** Particle to emit when this enemy dies */
	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitSpark;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float BulletVelocity;

	UFUNCTION(Category = Gameplay, BlueprintCallable)
	void ChangeBulletLevel(const int &bulletLevel);

	void SetLocationAndRotation(FVector location, FRotator rotation);

	void DeactivateEvent();

	/* Implementation of PoolableObjectInterface */
	void SetObjectLifeSpan_Implementation(float InLifespan) override;

	void SetActive_Implementation(bool IsActive) override;

	bool IsActive_Implementation() override;

	void Deactivate_Implementation() override;
private:
	float Lifespan = 5.0f;
	bool Active;
	FTimerHandle LifespanTimer;
	APawn *playerPawn;
	class AHiWaveGameState* hiWaveGameState;
};

