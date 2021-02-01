// Fill out your copyright notice in the Description page of Project Settings.


#include "RedEnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HiWavePawn.h"
#include "EnemyPawns/EnemyPawn.h"

// Sets default values
ARedEnemyProjectile::ARedEnemyProjectile()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	//ProjectileMesh->SetNotifyRigidBodyCollision(true);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ARedEnemyProjectile::OnHit);		// set up a notification for when this component hits something
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ARedEnemyProjectile::OnOverlapBegin);
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	BulletVelocity = 200.0f;
	ProjectileMovement->InitialSpeed = BulletVelocity;
	ProjectileMovement->MaxSpeed = BulletVelocity;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ARedEnemyProjectile::OnStop);

	Lifespan = 20.0;
	InitialLifeSpan = 0.0;
	Active = false;
}

// Called when the game starts or when spawned
void ARedEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	//FTimerDelegate destroyTimerDelegate;
	//FTimerHandle destroyTimerHandle;
	//destroyTimerDelegate.BindUFunction(this, FName("DeactivateEvent"));

	//GetWorld()->GetTimerManager().SetTimer(destroyTimerHandle, destroyTimerDelegate, Lifespan, false);
}

void ARedEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AEnemyPawn *enemy = Cast<AEnemyPawn>(OtherActor);
	if (enemy == nullptr) {
		if (DestroyParticle != nullptr) {
			FTransform transform = FTransform();
			transform.SetLocation(GetActorLocation());
			transform.SetRotation(FQuat::Identity);
			transform.SetScale3D(FVector::OneVector);

			spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyParticle, transform, true, EPSCPoolMethod::AutoRelease);
			spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (DestroySound != nullptr) {
				UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
			}
		}
		//DeactivateEvent();
		//GetWorldTimerManager().SetTimer(LifespanTimer, this, &ARedEnemyProjectile::DeactivateEvent, 0.5, false);
		DeactivateEvent();
	}
}


void ARedEnemyProjectile::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != NULL) {
		player->TakeHit();
	}

	AEnemyPawn *enemy = Cast<AEnemyPawn>(OtherActor);
	if (enemy == nullptr) {
		if (DestroyParticle != nullptr) {
			FTransform transform = FTransform();
			transform.SetLocation(GetActorLocation());
			transform.SetRotation(FQuat::Identity);
			transform.SetScale3D(FVector::OneVector);

			spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyParticle, transform, true, EPSCPoolMethod::AutoRelease);
			spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		//Destroy();
		//GetWorldTimerManager().SetTimer(LifespanTimer, this, &ARedEnemyProjectile::DeactivateEvent, 0.5, false);
		DeactivateEvent();

	}
}

void ARedEnemyProjectile::OnStop(const FHitResult & Hit)
{
	ProjectileMovement->SetUpdatedComponent(ProjectileMesh);
	ProjectileMovement->Velocity = FVector(0.0f, 0.0f, 0.0f);
	ProjectileMovement->UpdateComponentVelocity();
}

void ARedEnemyProjectile::DeactivateEvent()
{
	IPoolableObjectInterface::Execute_Deactivate(this);
}

void ARedEnemyProjectile::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ARedEnemyProjectile::DeactivateEvent, Lifespan, false);
}

void ARedEnemyProjectile::SetActive_Implementation(bool IsActive)
{
	Active = IsActive;
	if (IsActive) {
  		GetRootComponent()->SetVisibility(true);
		ProjectileMovement->InitialSpeed = BulletVelocity;
		ProjectileMovement->MaxSpeed = BulletVelocity;
		ProjectileMovement->Velocity = GetActorRotation().RotateVector(FVector::ForwardVector).GetSafeNormal() * BulletVelocity;
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		/*
		if (spawnedParticle != nullptr) {
			spawnedParticle->Deactivate();
		}
		*/
		GetRootComponent()->SetVisibility(false);
		SetActorLocation(FVector(0.0, 0.0, 2000.0));
		SetActorRotation(FRotator::ZeroRotator);
		ProjectileMovement->InitialSpeed = 0;
		ProjectileMovement->MaxSpeed = 0;
		ProjectileMovement->Velocity = FVector::ZeroVector;
	}
}

bool ARedEnemyProjectile::IsActive_Implementation()
{
	return Active;
}

void ARedEnemyProjectile::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}