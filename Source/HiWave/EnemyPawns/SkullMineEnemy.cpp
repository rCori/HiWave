// Fill out your copyright notice in the Description page of Project Settings.


#include "SkullMineEnemy.h"
#include "EnemyPawn.h"
#include "CollidingPawnMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnemyBarrierMarker.h"
#include "SkullMineWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "ItemPool.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"


ASkullMineEnemy::ASkullMineEnemy() : AEnemyPawn() {
	//Adding movement component
	OurMovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("CustomMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	BarrierCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BarrierDetection"));
	BarrierCollisionComponent->SetupAttachment(RootComponent);
	BarrierCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkullMineEnemy::OnBarrierOverlap);

	startingHealth = 20;
	health = startingHealth;
	speed = 500.0;
	pointsAwarded = 50;
	damageRatio = 1.0;
	burstAwarded = 0.5;
	damageRatioOnBurst = 2.0;

	directionChosen = EDiagonalDirection::VE_None;
	moveState = ESkullEnemyState::VE_Move;
	currentMovementDirection = FVector::ZeroVector;
	timeToMove = 4.0;
	timeToStop = 1.0;
	initializeDirectionalMap();
}

void ASkullMineEnemy::BeginPlay()
{

	auto staticMesh = FindComponentByClass<UStaticMeshComponent>();
	auto baseMaterial = staticMesh->GetMaterial(0);
	auto socketMaterial = staticMesh->GetMaterial(1);

	dynamicBaseMaterial = UMaterialInstanceDynamic::Create(baseMaterial, NULL);
	staticMesh->SetMaterial(0, dynamicBaseMaterial);

	dynamicSocketMaterial = UMaterialInstanceDynamic::Create(socketMaterial, NULL);
	staticMesh->SetMaterial(1, dynamicSocketMaterial);

	Super::BeginPlay();
}

void ASkullMineEnemy::Tick(float DeltaTime) {
	moveTimer -= DeltaTime;
	if (moveTimer <= 0.0) {
		TSet<EDiagonalDirection> directions = TSet<EDiagonalDirection>();
		EDiagonalDirection direction = EDiagonalDirection::VE_None;
		int selectedElement = 0;
		switch (moveState) {
		case ESkullEnemyState::VE_Bomb:
			directions.Add(EDiagonalDirection::VE_DownLeft);
			directions.Add(EDiagonalDirection::VE_DownRight);
			directions.Add(EDiagonalDirection::VE_UpLeft);
			directions.Add(EDiagonalDirection::VE_UpRight);
			selectedElement = FMath::RandRange(0, 3);
			direction = directions.Array()[selectedElement];

			directionChosen = direction;
			moveTimer = timeToMove;
			currentMovementDirection = directionalMap[directionChosen];
			moveState = ESkullEnemyState::VE_Move;
			break;
		case ESkullEnemyState::VE_Move:
			currentMovementDirection = FVector::ZeroVector;
			moveTimer = timeToStop;
			moveState = ESkullEnemyState::VE_Stop;
			break;
		case ESkullEnemyState::VE_Stop:
			if (itemPool == nullptr) {
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPool::StaticClass(), FoundActors);
				itemPool = Cast<AItemPool>(FoundActors[0]);
			}
			ASkullMineWeapon* mine = Cast<ASkullMineWeapon>(itemPool->GetPooledObject(EPoolableType::VE_SkullMineWeapon));
			if (mine != nullptr) {
				IPoolableObjectInterface::Execute_SetActive(mine, true);
				mine->SetActorLocationAndRotation(GetActorLocation(), FRotator::ZeroRotator);
			}
			moveState = ESkullEnemyState::VE_Bomb;
			break;
		}
	}
	AddMovementInput(currentMovementDirection, 1.0f);
}

void ASkullMineEnemy::EnemyDeath() {
	if (HitParticle != nullptr) {
		FRotator rotation = FRotator::ZeroRotator;
		FTransform transform = FTransform();
		transform.SetLocation(GetActorLocation());
		FQuat rotQuaternion = FQuat(rotation);
		transform.SetRotation(rotQuaternion);
		transform.SetScale3D(FVector::OneVector);
		spawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, transform, true, EPSCPoolMethod::AutoRelease);
		spawnedParticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Super::EnemyDeath();
}

void ASkullMineEnemy::BurstOverlap()
{
	dynamicBaseMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	dynamicSocketMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 1.0);
	Super::BurstOverlap();
}

void ASkullMineEnemy::SetActive_Implementation(bool IsActive)
{
	Active = IsActive;
	if (IsActive) {
		// Hides visible components
		SetActorHiddenInGame(false);
		// Disables collision components
		SetActorEnableCollision(true);
		// Stops the Actor from ticking
		SetActorTickEnabled(true);
		health = startingHealth;
		damageRatio = 1.0;
		dynamicBaseMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		dynamicSocketMaterial->SetScalarParameterValue(TEXT("IsHighlight"), 0.0);
		IPoolableObjectInterface::Execute_SetObjectLifeSpan(this, Lifespan);
	}
	else {
		// Hides visible components
		SetActorHiddenInGame(true);
		// Disables collision components
		SetActorEnableCollision(false);
		// Stops the Actor from ticking
		SetActorTickEnabled(false);
		OnEnemyDeathDelegate.Clear();
		OnIncreaseMultiplierDelegate.Clear();
		SetActorLocation(FVector(0.0, 0.0, 10000.0f));
		SetActorRotation(FRotator::ZeroRotator);
	}
}

void ASkullMineEnemy::OnBarrierOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AEnemyBarrierMarker *enemyBarrierMarker = Cast<AEnemyBarrierMarker>(OtherActor);
	if (enemyBarrierMarker != nullptr) {
		moveTimer = timeToMove;
		EDiagonalDirection validDirs[2] = { EDiagonalDirection::VE_None };
		EBarrierPosition barrierPosition = enemyBarrierMarker->GetBarrierPosition();
		switch (barrierPosition) {
		case EBarrierPosition::VE_Bottom:
			validDirs[0] = EDiagonalDirection::VE_UpLeft;
			validDirs[1] = EDiagonalDirection::VE_UpRight;
			break;
		case EBarrierPosition::VE_Top:
			validDirs[0] = EDiagonalDirection::VE_DownLeft;
			validDirs[1] = EDiagonalDirection::VE_DownRight;
			break;
		case EBarrierPosition::VE_Left:
			validDirs[0] = EDiagonalDirection::VE_DownRight;
			validDirs[1] = EDiagonalDirection::VE_UpRight;
			break;
		case EBarrierPosition::VE_Right:
			validDirs[0] = EDiagonalDirection::VE_DownLeft;
			validDirs[1] = EDiagonalDirection::VE_UpLeft;
			break;
		}
		int selectedElement = FMath::RandRange(0, 1);
		directionChosen = validDirs[selectedElement];
		currentMovementDirection = directionalMap[directionChosen];
	}
}

void ASkullMineEnemy::DoBombAttack() {
	UE_LOG(LogTemp, Warning, TEXT("DoBombAttack"));
}

void ASkullMineEnemy::initializeDirectionalMap() {
	directionalMap.Add(EDiagonalDirection::VE_UpLeft, FVector(1.0, -1.0, 0.0));
	directionalMap.Add(EDiagonalDirection::VE_UpRight, FVector(1.0, 1.0, 0.0));
	directionalMap.Add(EDiagonalDirection::VE_DownLeft, FVector(-1.0, -1.0, 0.0));
	directionalMap.Add(EDiagonalDirection::VE_DownRight, FVector(-1.0, 1.0, 0.0));
}
