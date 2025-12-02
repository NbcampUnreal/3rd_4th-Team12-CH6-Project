#include "GameInstance/SKGameInstance.h"
#include "Utility/SKBGMSubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Constants/SKGameConstants.h"


void USKGameInstance::HostTownSession()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UE_LOG(LogTemp, Warning, TEXT("NetMode before ServerTravel: %d"), (int32)World->GetNetMode());

	// 🔹 이미 네트워크 연결 중이면 호스트 불가
	if (World->GetNetMode() != NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Cannot host session: already in multiplayer mode."));
		return;
	}
	FString TravelCmd = FString::Printf(TEXT("%s?listen"), SKGameConstants::TownLevel);

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Hosting TownMap (Listen Server)... : %s"), *TravelCmd);
	UGameplayStatics::OpenLevel(GetWorld(), FName(TravelCmd), true, "listen");
	
}

void USKGameInstance::JoinHostTown(const FString& Address)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		UE_LOG(LogTemp, Log, TEXT("[GameInstance] Joining host at %s"), *Address);
		PC->ClientTravel(Address, TRAVEL_Absolute);
	}
}

void USKGameInstance::TravelToDungeon(int32 DungeonID)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	if (World->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] TravelToDungeon() called on Client — Ignored."));
		return;
	}

	const FString* LevelPath = SKGameConstants::DungeonLevels.Find(DungeonID);

	if (!LevelPath)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid DungeonID: %d"), DungeonID);
		return;
	}

	FString TravelCmd = FString::Printf(TEXT("%s?listen"), **LevelPath);
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel → DungeonMap : %s"), *TravelCmd);
	World->ServerTravel(TravelCmd, true);
}

void USKGameInstance::TravelToTown()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (World->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] TravelToTown() called on Client — Ignored."));
		return;
	}

	FString TravelCmd = FString::Printf(TEXT("%s?listen"), SKGameConstants::TownLevel);
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel → TownMap : %s"), *TravelCmd);
	World->ServerTravel(TravelCmd, true);
}

void USKGameInstance::LeaveSession()
{
	// 🔹 1. 세션 종료 로그
	if (GetWorld()->GetNetMode() != NM_Standalone)
	{
		UE_LOG(LogTemp, Log, TEXT("[GameInstance] Ending Multiplayer Session"));
	}

	// 🔹 2. 로컬 마을 레벨로 복귀 (세션 완전 종료)
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Returning to Local TownMap (Standalone)..."));
	FString TravelCmd = FString::Printf(TEXT("%s"), SKGameConstants::TownLevel);
	UGameplayStatics::OpenLevel(GetWorld(), FName(TravelCmd));
}



const TObjectPtr<USKSoundDataAsset>& USKGameInstance::GetSoundDataAsset()
{
	return SoundDataAsset;
}

void USKGameInstance::SetMasterVolume(float InVolume)
{
	MasterVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);

	if (USKBGMSubSystem* BGM = this->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->UpdateVolume();
	}
}

void USKGameInstance::SetBGMVolume(float InVolume)
{
	BGMVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);

	if (USKBGMSubSystem* BGM = this->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->UpdateVolume();
	}
}

void USKGameInstance::SetSFXVolume(float InVolume)
{
	SFXVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);
	
}
