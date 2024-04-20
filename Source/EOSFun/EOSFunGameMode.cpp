// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSFunGameMode.h"
#include "EOSFunCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEOSFunGameMode::AEOSFunGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
