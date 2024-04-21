// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_DELEGATE_TwoParams(FLoginCompletedDelegate, int, numOfPlayers, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateCompletedDelegate, FName, sessionName, bool, bWasSuccessful);

class IOnlineSubsystem;

UCLASS()
class EOSFUN_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: // Functions 

	UFUNCTION(BlueprintCallable)
	void Login();

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void FindSession(FString CodeFindSession);

	UFUNCTION(BlueprintCallable)
	void LoginCompletedDelegate(FLoginCompletedDelegate loginCompletedDelegate);

	UFUNCTION(BlueprintCallable)
	void CreateCompletedDelegate(FCreateCompletedDelegate createCompletedDelegate);

public: // Variables

	UPROPERTY(EditDefaultsOnly, Category = "Write")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY()
	FLoginCompletedDelegate OnLoginCompletedDelegate;

	UPROPERTY()
	FCreateCompletedDelegate OnCreateCompletedDelegate;

protected: // Functions
	virtual void Init() override;

private : // Functions
	void OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnCreateSessionCompleted(FName name, bool bWasSuccessful);
	void OnFindSessionCompleted(bool bWasSuccessful);
	void OnFindSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResultType);

	FString KeyGenerator();

private: // Variables

	IOnlineSubsystem* onlineSubsystem;

	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionPtr;

	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	const FName SessionNameKey{"SessionNameKey"};
	FString FindSessionName = "None";
	

public: //Get Set
	FORCEINLINE FName GetSessionNameKey() const { return SessionNameKey; }
	FORCEINLINE FString GetSessionName(const FOnlineSessionSearchResult& searchResult) const;
};
