// Fill out your copyright notice in the Description page of Project Settings.

#include "EOSGameInstance.h"
#include "Math/RandomStream.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

const char alphnum[] = "0123456789" "!@#$%^&*" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";
int strLen = sizeof(alphnum) - 1;

void UEOSGameInstance::Init()
{
	Super::Init();

	// Tro toi he thong online cua Unreal Engine 
	onlineSubsystem = IOnlineSubsystem::Get();

	// Tro toi giao dien dinh danh online 
	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleted);
	identityPtr->OnLogoutCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLogoutCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionCompleted);

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionCompleted);

	sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionCompleted);

	sessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionCompleted);
	
}

void UEOSGameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	if (identityPtr)
	{
		FOnlineAccountCredentials onlineAccountCredentials;
		onlineAccountCredentials.Type = LoginType;
		onlineAccountCredentials.Id = ID;
		onlineAccountCredentials.Token = Token;

		identityPtr->Login(0, onlineAccountCredentials);
	}
}


void UEOSGameInstance::LogoutEOS()
{
	if (identityPtr)
	{
		identityPtr->Logout(0);
	}
}


FString UEOSGameInstance::GetPlayerUsername()
{
	if (identityPtr)
	{
		// Lay thong tin nguoi choi hien tai
		// Lay ID cua nguoi choi dau tien (thuong la local player) 

		if (identityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn)
		{	
			// Lay thong tin chi tiet ve nguoi choi tu ID 
			// auto PlayerId = identityPtr->GetUniquePlayerId(0);

			FString PlayerName;
			PlayerName = identityPtr->GetPlayerNickname(0);

            // In ra ten cua nguoi choi 
            UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *PlayerName);
			return PlayerName;
		}
	}

	return FString();
}

bool UEOSGameInstance::IsPlayerLoggedIn()
{
	if (identityPtr)
	{
		if (identityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn)
		{
			return true;
		}
	}
	return false;
}

void UEOSGameInstance::CreateEOSSession(bool bIsDedicatedServer, int32 NumberOfPublicConnections)
{
	if (sessionPtr)
	{
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = true;
		sessionSettings.bIsDedicated = bIsDedicatedServer;
		sessionSettings.bIsLANMatch = false;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.bUsesPresence = true;
		sessionSettings.bAllowJoinInProgress = true;
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.NumPublicConnections = true;
		sessionSettings.NumPublicConnections = NumberOfPublicConnections;

		//passing an abitray data, and make is avaliable to be read on the client.
		FString SessionNameGenerator = KeyGenerator();
		//sessionSettings.Set(FName("LobbyName"), FString("MyFunLobby"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set(SessionNameKey, SessionNameGenerator, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		CurrentSessionName = FName(SessionNameGenerator);

		sessionPtr->CreateSession(0, FName(SessionNameGenerator), sessionSettings);
	}
}

void UEOSGameInstance::FindEOSSession(FString CodeFindSession)
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

void UEOSGameInstance::DestroyEOSSession()
{
	if (sessionPtr)
	{
		sessionPtr->DestroySession(CurrentSessionName);
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

void UEOSGameInstance::FindCompletedDelegate(FFindCompletedDelegate findCompletedDelegate)
{
	OnFindCompletedDelegate = findCompletedDelegate;
}

void UEOSGameInstance::JoinCompletedDelegate(FJoinCompletedDelegate joinCompletedDelegate)
{
	OnJoinCompletedDelegate = joinCompletedDelegate;
}

void UEOSGameInstance::DestroyCompletedDelegate(FDestroyCompletedDelegate destroyCompletedDelegate)
{
	OnDestroyCompletedDelegate = destroyCompletedDelegate;
}

void UEOSGameInstance::LogoutCompletedDelegate(FLogoutCompletedDelegate logoutCompletedDelegate)
{
	OnLogoutCompletedDelegate = logoutCompletedDelegate;
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

void UEOSGameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CurrentSessionName = SessionName;
		UE_LOG(LogTemp, Warning, TEXT("Session created name : %s"), *SessionName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Session created Successfully"))

		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session createtion failed"))
	}
	
	OnCreateCompletedDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
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
			UE_LOG(LogTemp, Warning, TEXT("Founded Session with id: %s"), *SessionName);
			UE_LOG(LogTemp, Warning, TEXT("Founding Session with id: %s"), *FindSessionName);

			if (FindSessionName == SessionName)
			{
				const FOnlineSessionSearchResult& searchResult = sessionSearch->SearchResults[KeyIndex];
				sessionPtr->JoinSession(0, FName(SessionName), searchResult);
				break;
			}
			KeyIndex++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Not Found with id: %s"), *FindSessionName);
	OnFindCompletedDelegate.ExecuteIfBound(bWasSuccessful);
}

void UEOSGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResultType)
{
	if (JoinResultType == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Successully"));
		FString TravelUrl;
		sessionPtr->GetResolvedConnectString(SessionName, TravelUrl);
		UE_LOG(LogTemp, Warning, TEXT("Join Session is : %s"), *TravelUrl);
		if (!TravelUrl.IsEmpty())
		{
			GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelUrl, ETravelType::TRAVEL_Absolute);
		}
		CurrentSessionName = SessionName;
		OnJoinCompletedDelegate.ExecuteIfBound(SessionName, "Success");
	}

	if (JoinResultType == EOnJoinSessionCompleteResult::SessionDoesNotExist)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Does Not Exist"));
		OnJoinCompletedDelegate.ExecuteIfBound(SessionName, "SessionDoesNotExist");
	}

	if (JoinResultType == EOnJoinSessionCompleteResult::SessionIsFull)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Is Full"));
		OnJoinCompletedDelegate.ExecuteIfBound(SessionName, "SessionIsFull");
	}

	if (JoinResultType == EOnJoinSessionCompleteResult::UnknownError)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session UnknownError"));
		OnJoinCompletedDelegate.ExecuteIfBound(SessionName, "UnknownError");
	}
}

void UEOSGameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Session Sussess is %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Session Failed is %s "), *SessionName.ToString());
	}

	OnDestroyCompletedDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
}

void UEOSGameInstance::OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logout Successed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Logout failed"));
	}

	OnLogoutCompletedDelegate.ExecuteIfBound(LocalUserNum, bWasSuccessful);
}

void UEOSGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> LevelToLoad)
{
	if (!LevelToLoad.IsValid())
	{
		LevelToLoad.LoadSynchronous();
	}
	if (LevelToLoad.IsValid())
	{
		const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));
		GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
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
