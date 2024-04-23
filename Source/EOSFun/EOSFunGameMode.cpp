// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSFunGameMode.h"
#include "EOSFunCharacter.h"
#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
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
	Super::PostLogin(NewPlayer);

	UEOSGameInstance* EOSGameInstance = GetGameInstance<UEOSGameInstance>();

	if (NewPlayer)
	{
		FUniqueNetIdRepl UniqueNetIDRepl;
		if (NewPlayer->IsLocalController())
		{
			ULocalPlayer* LocalPlayerRef = NewPlayer->GetLocalPlayer();

			if (LocalPlayerRef)
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
			IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
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
	}

}
