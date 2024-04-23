// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_DELEGATE_TwoParams(FLoginCompletedDelegate, int, numOfPlayers, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateCompletedDelegate, FName, sessionName, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFindCompletedDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FJoinCompletedDelegate, FName, sessionName, FString, JoinResultType);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDestroyCompletedDelegate, FName, sessionName, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLogoutCompletedDelegate, int32, LocalUserNum, bool, bWasSuccessful);

class IOnlineSubsystem;

UCLASS()
class EOSFUN_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected: // Functions
	virtual void Init() override;

private: // Functions
	void OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResultType);
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> LevelToLoad);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnLogoutCompleted(int32 LocalUserNum, bool bWasSuccessful);

	FString KeyGenerator();

private: // Variables

	IOnlineSubsystem* onlineSubsystem;

	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionPtr;

	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	const FName SessionNameKey{ "SessionNameKey" };
	FString FindSessionName = "None";

	FName CurrentSessionName;

public: // Functions 

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void CreateEOSSession(bool bIsDedicatedServer, int32 NumberOfPublicConnections);

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void FindEOSSession(FString CodeFindSession);

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void DestroyEOSSession();

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void LogoutEOS();
	/*
	* DELEGATE
	*/
	UFUNCTION(BlueprintCallable)
	void LoginCompletedDelegate(FLoginCompletedDelegate loginCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void CreateCompletedDelegate(FCreateCompletedDelegate createCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void FindCompletedDelegate(FFindCompletedDelegate findCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void JoinCompletedDelegate(FJoinCompletedDelegate joinCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void DestroyCompletedDelegate(FDestroyCompletedDelegate destroyCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void LogoutCompletedDelegate(FLogoutCompletedDelegate logoutCompletedDelegate);

public: // Variables
	/*
	* MAP
	*/
	UPROPERTY(EditDefaultsOnly, Category = "EOS Variables")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "EOS Variables")
	TSoftObjectPtr<UWorld> JudgeVoteLevel;

	/*
	* References
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Refs")
	APlayerController* Ref_PCMainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Refs")
	APlayerController* Ref_PCLobby;

	/*
	* DELEGATE
	*/
	UPROPERTY()
	FLoginCompletedDelegate OnLoginCompletedDelegate;

	UPROPERTY()
	FCreateCompletedDelegate OnCreateCompletedDelegate;

	UPROPERTY()
	FFindCompletedDelegate OnFindCompletedDelegate;

	UPROPERTY()
	FJoinCompletedDelegate OnJoinCompletedDelegate;

	UPROPERTY()
	FDestroyCompletedDelegate OnDestroyCompletedDelegate;

	UPROPERTY()
	FLogoutCompletedDelegate OnLogoutCompletedDelegate;

public: //Get Set
	FORCEINLINE FName GetSessionNameKey() const { return SessionNameKey; }
	FORCEINLINE FString GetSessionName(const FOnlineSessionSearchResult& searchResult) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	FORCEINLINE FName GetCurrentSessionName() const { return CurrentSessionName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	FORCEINLINE FString GetPlayerUsername();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOS Functions")
	FORCEINLINE bool IsPlayerLoggedIn();
};
