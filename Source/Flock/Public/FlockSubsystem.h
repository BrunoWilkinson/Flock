// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FlockSubsystem.generated.h"

UENUM()
enum class EFlockAction : uint8
{
	Create,
	Find,
	Join,
	Destroy,
	Start
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFLockOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FFlockOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FFlockOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlockOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlockOnStartSessionComplete, bool, bWasSuccessful);
DECLARE_LOG_CATEGORY_EXTERN(FlockLog, Error, All);

UCLASS()
class FLOCK_API UFlockSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFlockSubsystem();
	void CreateSession(const int32 NumPublicConnections, const FString MatchType);
	void FindSession(const int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	FFLockOnCreateSessionComplete FlockOnCreateSessionComplete;
	FFlockOnFindSessionComplete FlockOnFindSessionComplete;
	FFlockOnJoinSessionComplete FlockOnJoinSessionComplete;
	FFlockOnDestroySessionComplete FlockOnDestroySessionComplete;
	FFlockOnStartSessionComplete FlockOnStartSessionComplete;
	
protected:
	void OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful);
	void OnFindSessionComplete(const bool bWasSuccessful);
	void OnJoinSessionComplete(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(const FName SessionName, const bool bWasSuccessful);
	void OnStartSessionComplete(const FName SessionName, const bool bWasSuccessful);
	
private:
	bool IsValidSessionInterface() const;
	void LogFailure(const EFlockAction FlockAction);
	const ULocalPlayer* LocalPlayer;
	
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	FDelegateHandle FindSessionCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
