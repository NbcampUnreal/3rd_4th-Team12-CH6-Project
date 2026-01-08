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
}

void ADungeonGameState::OnRep_DungeonState()
{
	USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this);
	
	if (DungeonState == EDungeonMatchState::Dungeon_Loading) // 맵 진입 상태
	{ 
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_Loading"));
		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 0.6f;
			StateMessage.MessageText = FText::FromString(TEXT("던전 입장 중"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}
	}
	else if (DungeonState == EDungeonMatchState::Dungeon_PlayerReady) // 게임 시작 전 대기 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_PlayerReady"));
		// 대기 UI 표시, 입력 비활성화 등
		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 0.8f;
			StateMessage.MessageText = FText::FromString(TEXT("플레이어 준비 중"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}
	}
	else if (DungeonState == EDungeonMatchState::Dungeon_InProgress) //실제 게임 시작 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] DungeonState: Dungeon_InProgress"));

		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 1.0f;
			StateMessage.MessageText = FText::FromString(TEXT("던전 시작"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}
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
