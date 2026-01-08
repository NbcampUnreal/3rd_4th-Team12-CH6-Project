#include "GameState/TownGameState.h"
#include "GameFramework/GameMode.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

ATownGameState::ATownGameState()
{
	bReplicates = true;
}

void ATownGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATownGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	
	USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this);
	
	if (MatchState == MatchState::EnteringMap) // 맵 진입 상태
	{
		//EnteringMap는 OnRep_MatchState를 호출할수없음... 그냥 초기상태임
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: EnteringMap"));
		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 0.6f;
			StateMessage.MessageText = FText::FromString(TEXT("마을 입장 중"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}
	}
	else if (MatchState == MatchState::WaitingToStart) // 게임 시작 전 대기 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: WaitingToStart"));
		// 대기 UI 표시, 입력 비활성화 등
		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 0.8f;
			StateMessage.MessageText = FText::FromString(TEXT("플레이어 준비 중"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}
	}
	else if (MatchState == MatchState::InProgress) //실제 게임 시작 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: InProgress"));
		if (MessageSubsystem)
		{
			FLoadingStateMessage StateMessage;
			StateMessage.Progress = 1.0f;
			StateMessage.MessageText = FText::FromString(TEXT("마을 도착"));

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingInfo, StateMessage);
		}

	}
	else if (MatchState == MatchState::WaitingPostMatch) // 게임 결과 후 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: WaitingPostMatch"));

	}
	else if (MatchState == MatchState::LeavingMap) // 맵을 떠나는 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: LeavingMap"));
		// 세션종료, 레벨 전환 준비 UI 처리
	}
	else if (MatchState == MatchState::Aborted) // 강제 종료 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownGS] MatchState: Aborted"));
		// 정상적인 종료가 아닌, 강제로 경기가 중단된 상태일때 UI 처리
	}
}
