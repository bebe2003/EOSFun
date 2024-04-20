// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EOSFUN_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Login();
protected:
	virtual void Init() override;
private:

	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> identityPtr;
	void OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};
