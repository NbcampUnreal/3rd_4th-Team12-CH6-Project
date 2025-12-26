// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameMode/MatchState/DungeonMatchState.h"
#include "GameState/DungeonGameState.h"
#include "DungeonGameMode.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FDungeonStartedDelegate); //던전 시작 이벤트

UCLASS()
class SK_API ADungeonGameMode : public AGameMode
{
	GENERATED_BODY()
public:

	ADungeonGameMode();
	
	FDungeonStartedDelegate OnDungeonStarted;

	// PCG 완료 시 PCG에서 호출
	UFUNCTION(BlueprintCallable)
	void NotifyPCGFinished();

	// 초기 스폰 완료
	UFUNCTION(BlueprintCallable)
	void NotifyInitialSpawnFinished();

	// 현재 던전 상태 반환
	EDungeonMatchState GetDungeonState() const;

	UFUNCTION()
	void AddGoldToPlayers(int32 GoldValue);

	// 상태
	bool bPCGFinished = false;
	bool bInitialSpawnFinished = false;

protected:
	
#pragma region SeamlessTravel 관련
	
	// SeamlessTravel 관련
	virtual void PostSeamlessTravel() override; //플레이어의 맵이동 “시작” 이후
	virtual void HandleSeamlessTravelPlayer(AController*& C) override; //플레이어 맵 로드 후
#pragma endregion

	virtual void BeginPlay() override;
	
	//상태변경
	void ChangeDungeonState(EDungeonMatchState NewState);

	// 상태 흐름 처리
	void TryProgressState();

	// ★ 전체 플레이어 수 계산용
	void InitializePlayerCount();

private:
	// ListenServer 순서 문제 해결을 위한 지연 큐
	UPROPERTY()
	TArray<AController*> PendingReadyPlayers;

	// PostSeamlessTravel 호출 여부
	bool bPostSeamlessTravelCalled = false;

	// ★ PlayerReadyCount 증가 처리 함수
	void HandlePlayerReady(AController* C);

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
