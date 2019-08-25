// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HiWaveGameMode.h"
#include "HiWavePlayerController.h"
#include "HiWavePawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Json.h"

void AHiWaveGameMode::BeginPlay()
{
	FString JsonRaw = "{ \"exampleString\": \"Hello World\" }";
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		FString ExampleString = JsonParsed->GetStringField("exampleString");
		UE_LOG(LogTemp, Warning, TEXT("ExampleString: %s"), *ExampleString);
	}
}

AHiWaveGameMode::AHiWaveGameMode()
{
	// set default pawn class to our character class
	//DefaultPawnClass = AHiWavePawn::StaticClass();

	// This isn't working for some reason
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_HiWavePawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	

	// tell your custom game mode to use your custom player controller
	PlayerControllerClass = AHiWavePlayerController::StaticClass();

}

