// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DungeonGameState.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ADungeonGameState : public AGameState
{
	GENERATED_BODY()

public:
	ADungeonGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	virtual void OnRep_MatchState() override;
};
