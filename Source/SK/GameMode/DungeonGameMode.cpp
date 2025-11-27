#include "GameMode/DungeonGameMode.h"

ADungeonGameMode::ADungeonGameMode()
{
	bUseSeamlessTravel = true;
}

void ADungeonGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void ADungeonGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
}

bool ADungeonGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

void ADungeonGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	//게임 시작 하기 전 세팅해야할것들 구현 (ex : NPC 스폰)
}

void ADungeonGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	//게임 시작했을때
}

void ADungeonGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	
}

void ADungeonGameMode::NotifyPCGFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] PCG Finished"));
	bPCGFinished = true;
	TryStartDungeon();
}

void ADungeonGameMode::NotifyInitialSpawnFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Initial Monster Spawn Finished"));
	bInitialSpawnFinished = true;
	TryStartDungeon();
}

void ADungeonGameMode::TryStartDungeon()
{
	if (bDungeonStarted)
		return;

	// 두 조건 모두 충족해야 던전 시작
	if (bPCGFinished && bInitialSpawnFinished)
	{
		bDungeonStarted = true;

		UE_LOG(LogTemp, Warning, TEXT("[GameMode] Dungeon STARTED! Broadcasting Event"));

		// ★ 던전 시작 이벤트
		OnDungeonStarted.Broadcast();
	}
}