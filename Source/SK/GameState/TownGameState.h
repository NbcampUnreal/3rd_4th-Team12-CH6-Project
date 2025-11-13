// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TownGameState.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ATownGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATownGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	virtual void OnRep_MatchState() override;
};
