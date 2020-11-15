// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableObjectInterface.h"
#include "SkullMineWeapon.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UParticleSystemComponent;
class UParticleSystemComponent;

UCLASS()
class HIWAVE_API ASkullMineWeapon : public AActor, public IPoolableObjectInterface
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ASkullMineWeapon();

	UFUNCTION(BlueprintCallable)
	void Explode();

	UFUNCTION()
	virtual void OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	USphereComponent* SphereComponent;

	UPROPERTY(Category = Effects, EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ExplosionParticle;

	UPROPERTY(Category = Gameplay, BlueprintReadonly)
	UParticleSystemComponent* spawnedParticle;

	/** Sound to play each time enemy is hit */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundBase* DestroySound;

	void DeactivateEvent();

	/* Implementation of PoolableObjectInterface */
	void SetObjectLifeSpan_Implementation(float InLifespan) override;

	void SetActive_Implementation(bool IsActive) override;

	bool IsActive_Implementation() override;

	void Deactivate_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float Lifespan = 7.5f;
	bool Active;
	FTimerHandle LifespanTimer;

};
