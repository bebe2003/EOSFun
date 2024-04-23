// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EOSFunGameMode.generated.h"

UCLASS(minimalapi)
class AEOSFunGameMode : public AGameModeBase
{
	GENERATED_BODY()

	//AEOSFunGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

};



