#include "GameState/DungeonGameState.h"

#include "Net/UnrealNetwork.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"


ADungeonGameState::ADungeonGameState()
{
	bReplicates = true;
}

void ADungeonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonGameState, DungeonState);
	DOREPLIFETIME(ADungeonGameState, PlayerReadyCount);
	DOREPLIFETIME(ADungeonGameState, TotalPlayers);
}

void ADungeonGameState::SetDungeonState(EDungeonMatchState NewState)
{
	if (!HasAuthority()) return;

	DungeonState = NewState;
	OnDungeonMatchStateChanged.Broadcast(NewState);
	
	OnRep_DungeonState();

	if(DungeonState == EDungeonMatchState::Dungeon_InProgress)
	{
		if (UWorld* World = GetWorld())
		{
			if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
			{
				// 전송할 메시지 생성
				FLoadingUIVisible LoadingUIMessage(false);

				// 메시지 브로드캐스트 (UI 전환용 채널로)
				MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingUIVisible, LoadingUIMessage);
			}
		}
	}
}

void ADungeonGameState::OnRep_DungeonState()
{
	if (DungeonState == EDungeonMatchState::Dungeon_Loading) // 맵 진입 상태
	{ 
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_Loading"));
	}
	else if (DungeonState == EDungeonMatchState::Dungeon_PlayerReady) // 게임 시작 전 대기 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_PlayerReady"));
		// 대기 UI 표시, 입력 비활성화 등
	}
	else if (DungeonState == EDungeonMatchState::Dungeon_InProgress) //실제 게임 시작 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_InProgress"));
		
	}
	else if (DungeonState == EDungeonMatchState::Dungeon_Cleared) //던전 클리어
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_Cleared"));

	}
	else if (DungeonState == EDungeonMatchState::Dungeon_Failed) //던전 공략 실패
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_Failed"));

	}
}
