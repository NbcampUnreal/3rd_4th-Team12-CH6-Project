#include "GameMode/DungeonGameMode.h"
#include "Constants/SKGameConstants.h"

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

void ADungeonGameMode::ReturnToTown()
{
	FString TravelCmd = FString::Printf(TEXT("%s?listen"), SKGameConstants::TownLevel);
	UE_LOG(LogTemp, Warning, TEXT("Return To Town: %s"), *TravelCmd);

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TravelCmd, true); // SeamlessTravel
	}
}