#include "GameMode/TownGameMode.h"
#include "Constants/SKGameConstants.h"

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
}

void ATownGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	
}

void ATownGameMode::EnteringDungeon()
{
	FString TravelCmd = FString::Printf(TEXT("%s?listen"), SKGameConstants::DungeonLevel);
	UE_LOG(LogTemp, Warning, TEXT("Entering To Dungeon: %s"), *TravelCmd);

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TravelCmd, true); // SeamlessTravel
	}
}