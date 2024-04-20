// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

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

void UEOSGameInstance::Init()
{
	Super::Init();

	identityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();

	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginCompleted);
}

void UEOSGameInstance::OnLoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Successed"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login failed: %s"), *Error)
	}
}
