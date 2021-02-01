// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolableObjectInterface.h"
#include "GameFramework/Actor.h"
#include "RedEnemyProjectile.generated.h"


class UProjectileMovementComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class HIWAVE_API ARedEnemyProjectile : public AActor, public IPoolableObjectInterface
{
	GENERATED_BODY()
	
	/** Sphere collision component */
	UPROPERTY(EditDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:	
	// Sets default values for this actor's properties
	ARedEnemyProjectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStop(const FHitResult& Hit);

	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DestroyParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	/** Sound to play each time enemy is hit */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundBase* DestroySound;

	/* How fast red enemy projectiles move */
	UPROPERTY(Category = Gameplay, EditDefaultsOnly, BlueprintReadWrite)
	float BulletVelocity;

	void DeactivateEvent();

	/* Implementation of PoolableObjectInterface */
	void SetObjectLifeSpan_Implementation(float InLifespan) override;

	void SetActive_Implementation(bool IsActive) override;

	bool IsActive_Implementation() override;

	void Deactivate_Implementation() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float Lifespan;

private:
	bool Active;
	FTimerHandle LifespanTimer;
};
