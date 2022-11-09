// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FlockSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FLOCK_API UFlockSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFlockSubsystem();
	
protected:

private:
	IOnlineSessionPtr SessionInterface;
};
