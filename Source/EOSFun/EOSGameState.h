// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSGameState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSessionNameReplicated, FString, SessionName);

UCLASS()
class EOSFUN_API AEOSGameState : public AGameStateBase
{

	GENERATED_BODY()

protected:
	// Return properties that are replicated for the lifetime of the actor channcel
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	UFUNCTION()
	void OnRep_SessionName();

public: // Functions

	UFUNCTION(BlueprintCallable)
	FName GetSessionName() const { return SessionName; }

	UFUNCTION(BlueprintCallable)
	void SetSessionName(const FName& newSessionName);

	UFUNCTION(BlueprintCallable)
	void SessionNameReplicated(FOnSessionNameReplicated sessionNameReplicated);

public: // Variables
	UPROPERTY()
	FOnSessionNameReplicated OnSessionNameReplicated;
};
