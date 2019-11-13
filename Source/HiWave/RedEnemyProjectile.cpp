// Fill out your copyright notice in the Description page of Project Settings.


#include "RedEnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HiWavePawn.h"
#include "EnemyPawn.h"

// Sets default values
ARedEnemyProjectile::ARedEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	//ProjectileMesh->SetNotifyRigidBodyCollision(true);
	//ProjectileMesh->OnComponentHit.AddDynamic(this, &ARedEnemyProjectile::OnHit);		// set up a notification for when this component hits something
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ARedEnemyProjectile::OnOverlapBegin);
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 200.f;
	ProjectileMovement->MaxSpeed = 200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	// Die after 3 seconds by default
	InitialLifeSpan = 90.0f;

}

// Called when the game starts or when spawned
void ARedEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Red projectile was spawned"));
	
}

// Called every frame
void ARedEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
void ARedEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != NULL) {
		player->DoDeathAndRespawn();
	}

	AEnemyPawn *enemy = Cast<AEnemyPawn>(OtherActor);
	if (enemy == nullptr) {
		Destroy();
	}

}
*/

void ARedEnemyProjectile::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Only add impulse and destroy projectile if we hit a physics
	/*
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}
	*/

	AHiWavePawn *player = Cast<AHiWavePawn>(OtherActor);
	if (player != NULL) {
		player->TakeHit();
	}

	AEnemyPawn *enemy = Cast<AEnemyPawn>(OtherActor);
	if (enemy == nullptr) {
		Destroy();
	}
}
