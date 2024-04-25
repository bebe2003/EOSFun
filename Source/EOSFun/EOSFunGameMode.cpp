// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSFunGameMode.h"
#include "EOSFunCharacter.h"
#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/ConstructorHelpers.h"

//AEOSFunGameMode::AEOSFunGameMode()
//{
//	// set default pawn class to our Blueprinted character
//	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
//	if (PlayerPawnBPClass.Class != NULL)
//	{
//		DefaultPawnClass = PlayerPawnBPClass.Class;
//	}
//}

void AEOSFunGameMode::PostLogin(APlayerController* NewPlayer)
{

	UEOSGameInstance* EOSGameInstance = GetGameInstance<UEOSGameInstance>();

	check(IsValid(EOSGameInstance));

	if (IsValid(NewPlayer))
	{
		FUniqueNetIdRepl UniqueNetIDRepl;
		if (NewPlayer->IsLocalController())
		{
			ULocalPlayer* LocalPlayerRef = NewPlayer->GetLocalPlayer();

			if (IsValid(LocalPlayerRef))
			{
				UniqueNetIDRepl = LocalPlayerRef->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConnectionRef));
				UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
			}
		}
		else
		{
			UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnectionRef))
			UniqueNetIDRepl = RemoteNetConnectionRef->PlayerId;
		}
		
		TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIDRepl.GetUniqueNetId();

		if (UniqueNetId != nullptr)
		{
			//IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
			IOnlineSubsystem* onlineSubsystem = Online::GetSubsystem(NewPlayer->GetWorld());
			IOnlineSessionPtr sessionPtr = onlineSubsystem->GetSessionInterface();
			bool bRegistrationSuccess = sessionPtr->RegisterPlayer(EOSGameInstance->GetCurrentSessionName(), *UniqueNetId, false);
			if (bRegistrationSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Registration Sucessful"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Registration Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Registration UniqueNetId New Player not founded"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Registration New Player not founded"));
	}

	Super::PostLogin(NewPlayer);

}

void AEOSFunGameMode::PreLogout(APlayerController* InPlayerController)
{
	UEOSGameInstance* EOSGameInstance = GetGameInstance<UEOSGameInstance>();

	check(IsValid(EOSGameInstance));

	check(IsValid(InPlayerController));

	// This code handles logins for both the local player (listen server) and remote players (net connection).
	FUniqueNetIdRepl UniqueNetIdRepl;
	if (InPlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	// Get the unique player ID.
	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	// Get the online session interface.
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(InPlayerController->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// Unregister the player with the "MyLocalSessionName" session; this name should match the name you provided in CreateSession.
	if (Session->UnregisterPlayer(EOSGameInstance->GetCurrentSessionName(), *UniqueNetId))
	{
		UE_LOG(LogTemp, Warning, TEXT("UnregisterPlayer Sucessful"));
	}
	else
	{
		// The player could not be unregistered.
		UE_LOG(LogTemp, Warning, TEXT("UnregisterPlayer Failed"));
	}
}
