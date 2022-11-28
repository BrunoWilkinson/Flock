// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

DEFINE_LOG_CATEGORY(FlockLog);

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
	LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
}

void UFlockSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!IsValidSessionInterface())
	{
		FlockOnCreateSessionComplete.Broadcast(false);
		return;
	}

	const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		LogFailure(EFlockAction::Create);
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		FlockOnCreateSessionComplete.Broadcast(false);
	}
}

void UFlockSubsystem::FindSession(int32 MaxSearchResults)
{
	if(!IsValidSessionInterface())
	{
		FlockOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		LogFailure(EFlockAction::Find);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
		FlockOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UFlockSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if(!IsValidSessionInterface())
	{
		FlockOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		LogFailure(EFlockAction::Join);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		FlockOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UFlockSubsystem::DestroySession()
{
	if(!IsValidSessionInterface())
	{
		FlockOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if(!SessionInterface->DestroySession(NAME_GameSession))
	{
		LogFailure(EFlockAction::Destroy);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		FlockOnDestroySessionComplete.Broadcast(false);
	}
}

void UFlockSubsystem::StartSession()
{
	if(!IsValidSessionInterface())
	{
		FlockOnStartSessionComplete.Broadcast(false);
		return;
	}

	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		LogFailure(EFlockAction::Start);
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		FlockOnStartSessionComplete.Broadcast(false);
	}
}

void UFlockSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	FlockOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UFlockSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num())
	{
		FlockOnFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
		return;
	}
	FlockOnFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, false);
}

void UFlockSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	FlockOnJoinSessionComplete.Broadcast(Result);
	/*
	 * Need to return the an Address string from the delegate
	 * FString Address;
	 * SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
	 */
}

void UFlockSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	FlockOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UFlockSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}
	FlockOnStartSessionComplete.Broadcast(bWasSuccessful);
}

bool UFlockSubsystem::IsValidSessionInterface() const
{
	if (SessionInterface.IsValid())
	{
		return true;
	}
	UE_LOG(FlockLog, Error, TEXT("Invalid SessionInterface."));
	return false;
}

void UFlockSubsystem::LogFailure(const EFlockAction FlockAction)
{
	FString Action;
	switch (FlockAction)
	{
	case FlockAction == EFlockAction::Create:
		Action = TEXT("create");
		break;
	case FlockAction == EFlockAction::Find:
		Action = TEXT("find");
		break;
	case FlockAction == EFlockAction::Join:
		Action = TEXT("join");
		break;
	case FlockAction == EFlockAction::Destroy:
		Action = TEXT("destroy");
		break;
	case FlockAction == EFlockAction::Start:
		Action = TEXT("start");
		break;
	default:
		UE_LOG(FlockLog, Error, TEXT("Unkwown Flock Action."));
		return;
	}
	UE_LOG(FlockLog, Error, TEXT("Failed to %s a session."), *Action);
}
