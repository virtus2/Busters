// Copyright Epic Games, Inc. All Rights Reserved.

#include "BustersGameMode.h"
#include "BustersCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABustersGameMode::ABustersGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
