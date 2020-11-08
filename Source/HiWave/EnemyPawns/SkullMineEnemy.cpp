// Fill out your copyright notice in the Description page of Project Settings.


#include "SkullMineEnemy.h"
#include "EnemyPawn.h"
#include "CollidingPawnMovementComponent.h"
#include "EnemyBarrierMarker.h"
#include "SkullMineWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "ItemPool.h"
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
			UE_LOG(LogTemp, Warning, TEXT("SkullMineEnemy VE_Move"));
			break;
		case ESkullEnemyState::VE_Move:
			currentMovementDirection = FVector::ZeroVector;
			moveTimer = timeToStop;
			moveState = ESkullEnemyState::VE_Stop;
			UE_LOG(LogTemp, Warning, TEXT("SkullMineEnemy VE_Stop"));
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
				//bullet->SetActive(true);
				mine->SetActorLocationAndRotation(GetActorLocation(), FRotator::ZeroRotator);
			}
			moveState = ESkullEnemyState::VE_Bomb;
			UE_LOG(LogTemp, Warning, TEXT("SkullMineEnemy VE_Bomb"));
			break;
		}
	}
	AddMovementInput(currentMovementDirection, 1.0f);
}

void ASkullMineEnemy::EnemyDeath() {
	Super::EnemyDeath();
}

void ASkullMineEnemy::BurstOverlap()
{
	Super::BurstOverlap();
}

void ASkullMineEnemy::SetActive_Implementation(bool IsActive)
{
	//UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation in ASkullMineEnemy"));
	Active = IsActive;
	if (IsActive) {
		//UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation setting active true"));
		// Hides visible components
		SetActorHiddenInGame(false);
		// Disables collision components
		SetActorEnableCollision(true);
		// Stops the Actor from ticking
		SetActorTickEnabled(true);
		health = startingHealth;
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("SetActive_Implementation setting active false"));
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
		//UE_LOG(LogTemp, Warning, TEXT("SkullMineEnemy OnBarrierOverlap"));
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
