#include "GameState/DungeonGameState.h"
#include "GameFramework/GameMode.h"


ADungeonGameState::ADungeonGameState()
{
	bReplicates = true;
}

void ADungeonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADungeonGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	
	if (MatchState == MatchState::EnteringMap) // 맵 진입 상태
	{
		//EnteringMap는 OnRep_MatchState를 호출할수없음... 그냥 초기상태임
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: EnteringMap"));
	}
	else if (MatchState == MatchState::WaitingToStart) // 게임 시작 전 대기 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: WaitingToStart"));
		// 대기 UI 표시, 입력 비활성화 등
	}
	else if (MatchState == MatchState::InProgress) //실제 게임 시작 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: InProgress"));

	}
	else if (MatchState == MatchState::WaitingPostMatch) // 게임 결과 후 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: WaitingPostMatch"));

	}
	else if (MatchState == MatchState::LeavingMap) // 맵을 떠나는 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: LeavingMap"));
		// 세션종료, 레벨 전환 준비 UI 처리
	}
	else if (MatchState == MatchState::Aborted) // 강제 종료 상태
	{
		UE_LOG(LogTemp, Warning, TEXT("[DungeonGS] MatchState: Aborted"));
		// 정상적인 종료가 아닌, 강제로 경기가 중단된 상태일때 UI 처리
	}
}
