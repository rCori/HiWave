// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedEnemyProjectile.generated.h"


class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class HIWAVE_API ARedEnemyProjectile : public AActor
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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DestroySelf();

	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DestroyParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = Gameplay, EditDefaultsOnly)
	float lifespan;

};
