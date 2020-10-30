// Copyright 1998-2019 Epic Games, Inc. All Rights Reserve

#include "HiWaveProjectile.h"
#include "TimerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMesh.h"
#include "EnemyPawns/EnemyPawn.h"

AHiWaveProjectile::AHiWaveProjectile() 
{
	// Static reference to the mesh to use for the projectile
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Geometry/Meshes/PlayerBullet.PlayerBullet"));

	//Load the HitSpark particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Particles/PS_BasicEnemyHit.PS_BasicEnemyHit"));
	if (ParticleAsset.Succeeded())
	{
		HitSpark = ParticleAsset.Object;
	}

	BulletVelocity = 3000.0f;

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	//ProjectileMesh->SetNotifyRigidBodyCollision(true);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AHiWaveProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 2500.0f;
	ProjectileMovement->MaxSpeed = 2500.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AHiWaveProjectile::OnStop);

	// Die after 3 seconds by default
	//InitialLifeSpan = 3.0f;
	Lifespan = 5.0f;
	Active = false;
}

void AHiWaveProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	AEnemyPawn *enemyPawn = Cast<AEnemyPawn>(OtherActor);
	if (enemyPawn != NULL) {

		enemyPawn->EnemyTakeDamage(10.0f);

		if (HitSpark != nullptr) {
			FRotator rotation = FRotator::ZeroRotator;
			FTransform transform = FTransform();
			transform.SetLocation(Hit.Location);
			FQuat rotQuaternion = FQuat(rotation);
			transform.SetRotation(rotQuaternion);
			transform.SetScale3D(FVector::OneVector);
			spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitSpark, transform, true, EPSCPoolMethod::AutoRelease);
			spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	//Destroy();
	IPoolableObjectInterface::Execute_Deactivate(this);
	//Deactivate_Implementation();
}

void AHiWaveProjectile::OnStop(const FHitResult & Hit)
{
	ProjectileMovement->SetUpdatedComponent(ProjectileMesh);
	ProjectileMovement->Velocity = FVector(0.0f, 0.0f, 0.0f);
	ProjectileMovement->UpdateComponentVelocity();
}


void AHiWaveProjectile::SetLocationAndRotation(FVector location, FRotator rotation)
{
	SetActorRotation(rotation, ETeleportType::TeleportPhysics);
	SetActorLocation(location, false, nullptr, ETeleportType::TeleportPhysics);
	ProjectileMovement->InitialSpeed = BulletVelocity;
	ProjectileMovement->MaxSpeed = BulletVelocity;
	ProjectileMovement->Velocity = rotation.RotateVector(FVector::ForwardVector).GetSafeNormal() * BulletVelocity;
	GetRootComponent()->SetVisibility(true);
}

void AHiWaveProjectile::DeactivateEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("DeactivateEvent"));
	IPoolableObjectInterface::Execute_Deactivate(this);
}


void AHiWaveProjectile::SetObjectLifeSpan_Implementation(float InLifespan)
{
	Lifespan = InLifespan;
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &AHiWaveProjectile::DeactivateEvent, Lifespan, false);
}

void AHiWaveProjectile::SetActive_Implementation(bool IsActive)
{
	//Super::SetActive(IsActive);
	Active = IsActive;
	if (IsActive) {
		ProjectileMovement->InitialSpeed = BulletVelocity;
		ProjectileMovement->MaxSpeed = BulletVelocity;
		//SetObjectLifeSpan_Implementation(Lifespan);
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		GetRootComponent()->SetVisibility(false);
		SetActorLocation(FVector(0.0, 0.0, 2000.0));
		SetActorRotation(FRotator::ZeroRotator);
		ProjectileMovement->InitialSpeed = 0;
		ProjectileMovement->MaxSpeed = 0;
		ProjectileMovement->Velocity = FVector::ZeroVector;
	}
}

bool AHiWaveProjectile::IsActive_Implementation()
{
	return Active;
}

void AHiWaveProjectile::Deactivate_Implementation()
{
	IPoolableObjectInterface::Execute_SetActive(this, false);
	GetWorldTimerManager().ClearTimer(LifespanTimer);
}