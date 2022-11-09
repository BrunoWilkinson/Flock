// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockSubsystem.h"
#include "OnlineSubsystem.h"

UFlockSubsystem::UFlockSubsystem() :
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UFlockSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
}

void UFlockSubsystem::FindSession(int32 MaxSearchResults)
{
}

void UFlockSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UFlockSubsystem::DestroySession()
{
}

void UFlockSubsystem::StartSession()
{
}

void UFlockSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccesful)
{
}

void UFlockSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
}

void UFlockSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UFlockSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccesful)
{
}

void UFlockSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccesful)
{
}
