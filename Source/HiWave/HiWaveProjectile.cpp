// Copyright 1998-2019 Epic Games, Inc. All Rights Reserve

#include "HiWaveProjectile.h"
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
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
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

	Destroy();
}