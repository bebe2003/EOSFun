// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EOSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EOSFUN_API AEOSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnNetCleanup(UNetConnection* Connection) override;

};
