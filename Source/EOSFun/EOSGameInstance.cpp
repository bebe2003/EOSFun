// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

void UEOSGameInstance::Login()
{
	if (identityPtr)
	{
		FOnlineAccountCredentials onlineAccountCredentials;
		onlineAccountCredentials.Type = "AccountPortal";
		onlineAccountCredentials.Id = "";
		onlineAccountCredentials.Token = "";

		identityPtr->Login(0, onlineAccountCredentials);
	}
}

void UEOSGameInstance::CreateSession()
{
	if (sessionPtr)
	{
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = true;
		sessionSettings.bIsDedicated = false;
		sessionSettings.bIsLANMatch = false;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.bUsesPresence = true;
		sessionSettings.bAllowJoinInProgress = true;
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.NumPublicConnections = true;

		//passing an abitray data, and make is avaliable to be read on the client.
		sessionSettings.Set(FName("LobbyName"), FString("MyFunLobby"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		sessionPtr->CreateSession(0, "FunSession", sessionSettings);
	}
}

void UEOSGameInstance::LoginCompletedDelegate(FLoginCompletedDelegate loginCompletedDelegate)
{
	OnLoginCompletedDelegate = loginCompletedDelegate;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	onlineSubsystem = IOnlineSubsystem::Get();

	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionCompleted);
}

void UEOSGameInstance::OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Successed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login failed: %s"), *Error);
	}

	OnLoginCompletedDelegate.ExecuteIfBound(numOfPlayers, bWasSuccessful);
}

void UEOSGameInstance::OnCreateSessionCompleted(FName name, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session created Successfully"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session createtion failed"))
	}
	
	if (!LobbyLevel.IsValid())
	{
		LobbyLevel.LoadSynchronous();
	}
	if (LobbyLevel.IsValid())
	{
		const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LobbyLevel.ToString()));
		GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
	}
}
