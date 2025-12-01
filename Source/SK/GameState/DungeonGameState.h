// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameMode/MatchState/DungeonMatchState.h"
#include "DungeonGameState.generated.h"

/**
 * 
 */

// 던전 레벨 상태 변경 알림 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FDungeonMatchStateChanged, EDungeonMatchState);

UCLASS()
class SK_API ADungeonGameState : public AGameState
{
	GENERATED_BODY()

public:
	ADungeonGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetDungeonState(EDungeonMatchState NewState);

	// 현재 던전 상태 (Replicated)
	UPROPERTY(ReplicatedUsing=OnRep_DungeonState, BlueprintReadOnly)
	EDungeonMatchState DungeonState = EDungeonMatchState::None;

	// ★ 델리게이트 (AIController, HUD, SpawnSubsystem 등이 구독할 수 있음)
	FDungeonMatchStateChanged OnDungeonMatchStateChanged;

	//추가: SeamlessTravel 후 준비된 플레이어 수
	UPROPERTY(Replicated)
	int32 PlayerReadyCount = 0;

	//총 플레이어 수 (초기화는 GameMode가 수행)
	UPROPERTY(Replicated)
	int32 TotalPlayers = 0;

protected:
	
	UFUNCTION()
	void OnRep_DungeonState();
};
