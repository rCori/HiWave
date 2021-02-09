// Fill out your copyright notice in the Description page of Project Settings.


#include "SkullMineWeapon.h"
#include "TimerManager.h"
#include "HiWavePawn.h"
#include "Kismet/GameplayStatics.h"
#include "HiWaveProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASkullMineWeapon::ASkullMineWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkullMineWeapon::OnOverlap);
	RootComponent = SphereComponent;
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);


	MineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	MineMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MineMesh->SetupAttachment(RootComponent);

	Active = false;
}

// Called when the game starts or when spawned
void ASkullMineWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASkullMineWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkullMineWeapon::Explode() {
	if (ExplosionParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		FTransform transform = FTransform();
		transform.SetLocation(GetActorLocation());
		FQuat rotQuaternion = FQuat(rotation);
		transform.SetRotation(rotQuaternion);
		transform.SetScale3D(FVector::OneVector);
		spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, transform, true, EPSCPoolMethod::AutoRelease);
		spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	DeactivateEvent();
}

void ASkullMineWeapon::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AHiWavePawn *playerActor = Cast<AHiWavePawn>(OtherActor);
	if (playerActor != NULL && OtherComp->ComponentHasTag("ShipMesh")) {
		playerActor->TakeHit();
		Explode();
	}

	//Destroy mines that have been hit with a bullet
	AHiWaveProjectile *playerProjectile = Cast<AHiWaveProjectile>(OtherActor);
	if (playerProjectile != nullptr) {
		//UE_LOG(LogTemp, Warning, TEXT("Player shot a mine and blew it up"));
		if (DestroySound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
		}
		Explode();
		IPoolableObjectInterface::Execute_Deactivate(playerProjectile);
	}
}

void ASkullMineWeapon::DeactivateEvent()
{
	IPoolableObjectInterface::Execute_Deactivate(this);
}

void ASkullMineWeapon::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ASkullMineWeapon::Explode, Lifespan, false);
}

void ASkullMineWeapon::SetActive_Implementation(bool IsActive)
{
	Active = IsActive;
	if (IsActive) {
		// Hides visible components
		SetActorHiddenInGame(false);
		// Disables collision components
		SetActorEnableCollision(true);
		// Stops the Actor from ticking
		SetActorTickEnabled(true);
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		// Hides visible components
		SetActorHiddenInGame(true);
		// Disables collision components
		SetActorEnableCollision(false);
		// Stops the Actor from ticking
		SetActorTickEnabled(false);
		SetActorLocation(FVector(0.0, 0.0, 10000.0f));
		SetActorRotation(FRotator::ZeroRotator);
	}
}

bool ASkullMineWeapon::IsActive_Implementation()
{
	return Active;
}

void ASkullMineWeapon::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}
