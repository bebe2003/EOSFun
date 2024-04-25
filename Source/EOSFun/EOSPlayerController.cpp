// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSPlayerController.h"
#include "EOSGameInstance.h"
#include "EOSFunGameMode.h"

void AEOSPlayerController::OnNetCleanup(UNetConnection* Connection)
{
	//if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	
	UEOSGameInstance* GameInstanceRef = Cast<UEOSGameInstance>(GetWorld()->GetGameInstance());
	AEOSFunGameMode* GameModeRef = Cast<AEOSFunGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeRef)
	{
		GameModeRef->PreLogout(this);
	}

	// Giai quyet bai toan check Authority hay la Client
	if (GameInstanceRef && GameInstanceRef->IsPlayerHosting())
	{
		GameInstanceRef->DestroyEOSSession();
	}
	
	Super::OnNetCleanup(Connection);
}
