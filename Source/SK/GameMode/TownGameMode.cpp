#include "GameMode/TownGameMode.h"

#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

ATownGameMode::ATownGameMode()
{
	bUseSeamlessTravel = true;
}

void ATownGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void ATownGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
}

bool ATownGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

void ATownGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	//게임 시작 하기 전 세팅해야할것들 구현 (ex : NPC 스폰)
}

void ATownGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	//게임 시작했을때

	if (UNetDriver* ND = GetWorld()->GetNetDriver())
	{
		UE_LOG(LogTemp, Log, TEXT("✅ NetDriver Active"));
		UE_LOG(LogTemp, Log, TEXT("  • Class: %s"), *ND->GetClass()->GetName());
		UE_LOG(LogTemp, Log, TEXT("  • World: %s"), *ND->GetWorld()->GetName());
		UE_LOG(LogTemp, Log, TEXT("  • Network Address: %s"), *ND->LowLevelGetNetworkNumber());

		switch (ND->GetNetMode())
		{
		case NM_Standalone:
			UE_LOG(LogTemp, Log, TEXT("  • NetMode: Standalone")); break;
		case NM_ListenServer:
			UE_LOG(LogTemp, Log, TEXT("  • NetMode: ListenServer")); break;
		case NM_DedicatedServer:
			UE_LOG(LogTemp, Log, TEXT("  • NetMode: DedicatedServer")); break;
		case NM_Client:
			UE_LOG(LogTemp, Log, TEXT("  • NetMode: Client")); break;
		default:
			UE_LOG(LogTemp, Log, TEXT("  • NetMode: Unknown")); break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ NetDriver is NULL — no socket listening"));
	}
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

void ATownGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	
}