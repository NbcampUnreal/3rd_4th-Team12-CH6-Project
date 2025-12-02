#include "GameMode/DungeonGameMode.h"

ADungeonGameMode::ADungeonGameMode()
{
	bUseSeamlessTravel = true;
}

void ADungeonGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	ChangeDungeonState(EDungeonMatchState::Dungeon_Loading);

	bPostSeamlessTravelCalled = true;
	
	InitializePlayerCount();

	for (AController* C : PendingReadyPlayers)
	{
		HandlePlayerReady(C);
	}

	PendingReadyPlayers.Empty();
}

void ADungeonGameMode::InitializePlayerCount()
{
	auto* GS = GetGameState<ADungeonGameState>();
	if (!GS) return;

	// PlayerState 수 == 전체 플레이어 수
	GS->TotalPlayers = GS->PlayerArray.Num();

	UE_LOG(LogTemp, Warning,
		TEXT("[DungeonGameMode] TotalPlayers = %d"),
		GS->TotalPlayers);
}

void ADungeonGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	auto* GS = GetGameState<ADungeonGameState>();
	if (!GS) return;

	if (!bPostSeamlessTravelCalled)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGameMode] Queued player until PostSeamlessTravel"));

		PendingReadyPlayers.Add(C);
		return;
	}

	// 이미 PlayerCount 초기화 완료 → 즉시 처리
	HandlePlayerReady(C);
}

void ADungeonGameMode::HandlePlayerReady(AController* C)
{
	auto* GS = GetGameState<ADungeonGameState>();
	if (!GS) return;

	GS->PlayerReadyCount++;

	UE_LOG(LogTemp, Warning,
		TEXT("[DungeonGameMode] Player Ready: %d / %d"),
		GS->PlayerReadyCount, GS->TotalPlayers);

	// 모든 플레이어 로딩 완료
	if (GS->PlayerReadyCount >= GS->TotalPlayers)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGameMode] All Players Ready!"));
		ChangeDungeonState(EDungeonMatchState::Dungeon_PlayerReady);
		TryProgressState();
	}
}

EDungeonMatchState ADungeonGameMode::GetDungeonState() const
{
	if (auto* GS = GetGameState<ADungeonGameState>())
		return GS->DungeonState;

	return EDungeonMatchState::None;
}

void ADungeonGameMode::ChangeDungeonState(EDungeonMatchState NewState)
{
	auto* GS = GetGameState<ADungeonGameState>();
	if (!GS) return;

	GS->SetDungeonState(NewState);

	if (NewState == EDungeonMatchState::Dungeon_InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGameMode] Dungeon Started!"));
		OnDungeonStarted.Broadcast();
	}
}

void ADungeonGameMode::NotifyPCGFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] PCG Finished"));
	bPCGFinished = true;
	TryProgressState();
}

void ADungeonGameMode::NotifyInitialSpawnFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Initial Monster Spawn Finished"));
	bInitialSpawnFinished = true;
	TryProgressState();
}

void ADungeonGameMode::TryProgressState()
{
	bool bPlayersReady = (GetDungeonState() == EDungeonMatchState::Dungeon_PlayerReady);

	UE_LOG(LogTemp, Warning, TEXT("ADungeonGameMode::TryProgressState() - bPCGFinished : %d,   bInitialSpawnFinished : %d,   bPlayersReady : %d"), bPCGFinished, bInitialSpawnFinished, bPlayersReady);
	
	// PCG 완료 + 초기 스폰 완료 → DungeonReady
	if (bPCGFinished && bInitialSpawnFinished && bPlayersReady)
	{
		// 바로 전투 시작
		ChangeDungeonState(EDungeonMatchState::Dungeon_InProgress);
	}
}
