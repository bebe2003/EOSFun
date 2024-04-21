// Fill out your copyright notice in the Description page of Project Settings.

#include "Math/RandomStream.h"
#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

const char alphnum[] = "0123456789" "!@#$%^&*" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";
int strLen = sizeof(alphnum) - 1;

void UEOSGameInstance::Init()
{
	Super::Init();

	onlineSubsystem = IOnlineSubsystem::Get();

	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionCompleted);

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionCompleted);

	sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionCompleted);

}

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
		sessionSettings.NumPublicConnections = 10;

		//passing an abitray data, and make is avaliable to be read on the client.
		FString SessionNameGenerator = KeyGenerator();
		//sessionSettings.Set(FName("LobbyName"), FString("MyFunLobby"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set(SessionNameKey, SessionNameGenerator, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		sessionPtr->CreateSession(0, "FunSession", sessionSettings);
	}
}

void UEOSGameInstance::FindSession(FString CodeFindSession)
{
	if (sessionPtr)
	{
		sessionSearch = MakeShareable(new FOnlineSessionSearch);

		sessionSearch->bIsLanQuery = false;
		sessionSearch->MaxSearchResults = 100;
		sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals); //SEARCH_KEYWORDS
		
		FindSessionName = CodeFindSession;
		sessionPtr->FindSessions(0, sessionSearch.ToSharedRef());
	}
}

void UEOSGameInstance::LoginCompletedDelegate(FLoginCompletedDelegate loginCompletedDelegate)
{
	OnLoginCompletedDelegate = loginCompletedDelegate;
}

void UEOSGameInstance::CreateCompletedDelegate(FCreateCompletedDelegate createCompletedDelegate)
{
	OnCreateCompletedDelegate = createCompletedDelegate;
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

	OnCreateCompletedDelegate.ExecuteIfBound(name, bWasSuccessful);
}

void UEOSGameInstance::OnFindSessionCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful && sessionPtr && sessionSearch->SearchResults.Num() > 0)
	{
		int KeyIndex = 0;
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults)
		{
			FString SessionName = GetSessionName(lobbyFound);
			//UE_LOG(LogTemp, Warning, TEXT("Found Session with id: %s"), *lobbyFound.GetSessionIdStr());
			UE_LOG(LogTemp, Warning, TEXT("Found Session with id: %s"), *SessionName);

			if (FindSessionName == SessionName)
			{
				const FOnlineSessionSearchResult& searchResult = sessionSearch->SearchResults[KeyIndex];
				sessionPtr->JoinSession(0, FName(SessionName), searchResult);
				break;
			}
			KeyIndex++;
		}
	}
}

void UEOSGameInstance::OnFindSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResultType)
{
	if (JoinResultType == EOnJoinSessionCompleteResult::Success)
	{
		FString TravelUrl;
		sessionPtr->GetResolvedConnectString("", TravelUrl);
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelUrl, ETravelType::TRAVEL_Absolute);
	}
}

FString UEOSGameInstance::KeyGenerator()
{
	FString keys;
	char C;
	for (int i = 0; i < 8; i++)
	{
		C = alphnum[FMath::Rand() % strLen];
		keys += C;
	}

	return keys;
}

FString UEOSGameInstance::GetSessionName(const FOnlineSessionSearchResult& searchResult) const
{
	FString outVal = { "None" };
	searchResult.Session.SessionSettings.Get(SessionNameKey, outVal);
	return outVal;
}
