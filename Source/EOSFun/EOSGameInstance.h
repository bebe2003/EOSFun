// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EOSGameInstance.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_DELEGATE_TwoParams(FLoginCompletedDelegate, int, numOfPlayers, bool, bWasSuccessful);


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
	void LoginCompletedDelegate(FLoginCompletedDelegate loginCompletedDelegate);


public: // Variables

	UPROPERTY(EditDefaultsOnly, Category = "Write")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Read")
	bool bLoginSuccess = false;

	UPROPERTY()
	FLoginCompletedDelegate OnLoginCompletedDelegate;

protected: // Functions
	virtual void Init() override;

private: // Variables

	IOnlineSubsystem* onlineSubsystem;

	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionPtr;

private : // Functions
	void OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnCreateSessionCompleted(FName name, bool bWasSuccessful);
};
