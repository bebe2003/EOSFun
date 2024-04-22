// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameState.h"
#include "Net/UnrealNetwork.h"

void AEOSGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSGameState, SessionName, COND_None, REPNOTIFY_Always);
}

void AEOSGameState::SetSessionName(const FName& newSessionName)
{
	SessionName = newSessionName;
}

void AEOSGameState::OnRep_SessionName()
{
	OnSessionNameReplicated.ExecuteIfBound(SessionName.ToString());
}

void AEOSGameState::SessionNameReplicated(FOnSessionNameReplicated sessionNameReplicated)
{
	OnSessionNameReplicated = sessionNameReplicated;
}
