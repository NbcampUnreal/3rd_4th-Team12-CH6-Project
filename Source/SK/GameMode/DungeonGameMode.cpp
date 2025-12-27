#include "GameMode/DungeonGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/SKPlayerState.h"
#include "Item/Bonfire/SKBonfire.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Utility/SpawnSubsystem.h"

ADungeonGameMode::ADungeonGameMode()
{
	bUseSeamlessTravel = true;
}

void ADungeonGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ADungeonGameMode::BeginPlay"));

	UWorld* World = GetWorld();
	if (!World) return;

	USpawnSubsystem* SpawnSubsystem = World->GetSubsystem<USpawnSubsystem>();
	if (!SpawnSubsystem) return;

	SpawnSubsystem->SpawnAll();
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

AActor* ADungeonGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> Bonfires;
	UGameplayStatics::GetAllActorsOfClass(this, ASKBonfire::StaticClass(), Bonfires);

	for (AActor* Actor : Bonfires)
	{
		ASKBonfire* Bonfire = Cast<ASKBonfire>(Actor);
		if (Bonfire && Bonfire->bIsDefaultBonfire)
		{
			ASKPlayerState* PS = Player->GetPlayerState<ASKPlayerState>();
			if (PS)
			{
				PS->CurrentBonfire = Bonfire;
			}

			FVector SpawnLocation = Bonfire->InteractionPoint->GetComponentLocation();
			FRotator SpawnRotation = Bonfire->InteractionPoint->GetComponentRotation();

			APlayerStart* TempStart = GetWorld()->SpawnActor<APlayerStart>(
				APlayerStart::StaticClass(),
				SpawnLocation,
				SpawnRotation
			);
			
			return TempStart;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
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

	//UE_LOG(LogTemp, Warning, TEXT("ADungeonGameMode::TryProgressState() - bPCGFinished : %d,   bInitialSpawnFinished : %d,   bPlayersReady : %d"), bPCGFinished, bInitialSpawnFinished, bPlayersReady);
	UE_LOG(LogTemp, Warning, TEXT("ADungeonGameMode::TryProgressState() - bInitialSpawnFinished : %d,   bPlayersReady : %d"), bInitialSpawnFinished, bPlayersReady);

	//임시
	bPlayersReady = true;
	
	// PCG 완료 + 초기 스폰 완료 → DungeonReady
	//if (bPCGFinished && bInitialSpawnFinished && bPlayersReady)

	//초기 스폰 완료 → DungeonReady
	if (bInitialSpawnFinished && bPlayersReady)
	{
		// 바로 전투 시작
		ChangeDungeonState(EDungeonMatchState::Dungeon_InProgress);
	}
}

void ADungeonGameMode::AddGoldToPlayers(int32 GoldValue)
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
		if (PS)
		{
			PS->AddGold(GoldValue);
		}
	}
}