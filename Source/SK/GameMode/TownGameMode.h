// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameState/TownGameState.h"
#include "TownGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ATownGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ATownGameMode();

	FORCEINLINE ATownGameState* GetGameState() { return Super::GetGameState<ATownGameState>(); }

	UFUNCTION(BlueprintCallable)
	void EnteringDungeon();

	

protected:
	
#pragma region SeamlessTravel 관련
	
	// SeamlessTravel 관련
	virtual void PostSeamlessTravel() override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
#pragma endregion

#pragma region 게임 상태 변화에 따른 로직

	//StartMatch()가 호출됐을 때 WaitingToStart 에서 InProgress 로 넘어가기 전에 넘어가도 되는지 판단 (bool 값)
	virtual bool ReadyToStartMatch_Implementation() override;

	//MatchState 가 WaitingToStart로 바뀔 때 호출
	virtual void HandleMatchIsWaitingToStart() override;

	//StartMatch()가 성공해서 MatchState::InProgress 로 진입할 때 호출
	virtual void HandleMatchHasStarted() override;

	//EndMatch()가 호출되어 MatchState::WaitingPostMatch 로 바뀔 때 실행
	virtual void HandleMatchHasEnded() override;
#pragma endregion
};
