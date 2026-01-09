#include "GameInstance/SKGameInstance.h"

#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKBGMSubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Constants/SKGameConstants.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacter.h"
#include "PlayerState/SKPlayerState.h"
#include "EngineUtils.h"

void USKGameInstance::Init()
{
	Super::Init();
	
	USKGameplayMessageSubsystem* MessageSubsystem = GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	LoadingUIVisibleHandle = MessageSubsystem->RegisterListener<FLoadingUIVisible>(
		TAG_Message_Channel_LoadingUIVisible,
		this,
		&USKGameInstance::OnLoadingUIVisibleMessageReceived
	);

	RandomNormal = 0;
}

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
	
	if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
	{
		// 전송할 메시지 생성
		FLoadingUIVisible LoadingUIMessage(true);
		// 메시지 브로드캐스트 (UI 전환용 채널로)
		MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingUIVisible, LoadingUIMessage);
	}
	
	FString TravelCmd = FString::Printf(TEXT("%s?listen"), **LevelPath);
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel → DungeonMap : %s"), *TravelCmd);

	for (TActorIterator<ASKPlayerCharacter> It(GetWorld()); It; ++It)
	{
		ASKPlayerCharacter* Player = *It;
		if (!IsValid(Player)) continue;

		Player->PreDestroyGAS();
	}
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([World, TravelCmd]()
		{
			if (World)
			{
				UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel executing → TownMap : %s"), *TravelCmd);
				World->ServerTravel(TravelCmd, true);
			}
		}),
		1.0f,  // 1초 지연
		false
	);
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
		
	if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
	{
		// 전송할 메시지 생성
		FLoadingUIVisible LoadingUIMessage(true);
		// 메시지 브로드캐스트 (UI 전환용 채널로)
		MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingUIVisible, LoadingUIMessage);
		RandomNormal = 1;
	}

	for (TActorIterator<ASKPlayerCharacter> It(GetWorld()); It; ++It)
	{
		ASKPlayerCharacter* Player = *It;
		if (!IsValid(Player)) continue;

		Player->PreDestroyGAS();
	}

	for (TActorIterator<ASKAICharacter> It(GetWorld()); It; ++It)
	{
		ASKAICharacter* AI = *It;
		if (!IsValid(AI)) continue;

		AI->PreDestroyGAS();
		AI->Destroy();
	}

	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([World, TravelCmd]()
		{
			if (World)
			{
				UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel executing → TownMap : %s"), *TravelCmd);
				World->ServerTravel(TravelCmd, true);
			}
		}),
		1.0f,  // 1초 지연
		false
	);
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

void USKGameInstance::OpenTownEnding()
{
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Ending Multiplayer Session"));
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		this,
		&USKGameInstance::TravelToEnding,
		5.0f,
		false
	);
}

void USKGameInstance::TravelToEnding()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (World->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] TravelToTown() called on Client — Ignored."));
		return;
	}
	
	FString TravelCmd = FString::Printf(TEXT("%s?listen"), SKGameConstants::TownEnding);
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel → TownMap : %s"), *TravelCmd);

	if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
	{
		// 전송할 메시지 생성
		FLoadingUIVisible LoadingUIMessage(true);
		RandomNormal = 1;
			// 메시지 브로드캐스트 (UI 전환용 채널로)
		MessageSubsystem->BroadcastMessage(TAG_Message_Channel_LoadingUIVisible, LoadingUIMessage);
	}

	for (TActorIterator<ASKPlayerCharacter> It(GetWorld()); It; ++It)
	{
		ASKPlayerCharacter* Player = *It;
		if (!IsValid(Player)) continue;

		Player->PreDestroyGAS();
	}

	for (TActorIterator<ASKAICharacter> It(GetWorld()); It; ++It)
	{
		ASKAICharacter* AI = *It;
		if (!IsValid(AI)) continue;

		AI->PreDestroyGAS();
		AI->Destroy();
	}
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([World, TravelCmd]()
		{
			if (World)
			{
				UE_LOG(LogTemp, Log, TEXT("[GameInstance] ServerTravel executing → TownMap : %s"), *TravelCmd);
				World->ServerTravel(TravelCmd, true);
			}
		}),
		1.0f,  // 1초 지연
		false
	);
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

void USKGameInstance::ShowLoadingScreen(bool bShow)
{
	
	if (!GEngine || !GEngine->GameViewport)
	{
		UE_LOG(LogTemp, Error, TEXT("[SKGameInstance] No GameViewport found!"));
		return;
	}
	
	APlayerController* PC = GetFirstLocalPlayerController();
	FString PCName = PC ? PC->GetName() : TEXT("None");

	UE_LOG(LogTemp, Log, TEXT("[SKGameInstance] ShowLoadingScreen called. bShow=%s, PC=%s"),
		bShow ? TEXT("true") : TEXT("false"), *PCName);
	if (bShow)
	{
		if (!LoadingWidgetInstance && LoadingWidgetClass)
		{
			// PlayerController는 입력 목적용, 위젯은 GameInstance 소유
			LoadingWidgetInstance = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
			if (LoadingWidgetInstance)
			{
				// Slate 위젯으로 변환 후 GameViewport에 직접 추가
				LoadingSlateWidget = LoadingWidgetInstance->TakeWidget();
				GEngine->GameViewport->AddViewportWidgetContent(LoadingSlateWidget.ToSharedRef(), 9999);
			}
		}
	}
	else
	{
		if (LoadingSlateWidget.IsValid())
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(LoadingSlateWidget.ToSharedRef());
			LoadingSlateWidget.Reset();
		}

		if (LoadingWidgetInstance)
		{
			LoadingWidgetInstance = nullptr;
		}
	}
	
}


void USKGameInstance::OnLoadingUIVisibleMessageReceived(FGameplayTag Channel, const FLoadingUIVisible& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[USKGameInstance] ShowLoadingScreen %d"), Message.bVisible);

	if (Message.bVisible)
	{
		ShowLoadingScreen(true);
	}
	else
	{
		ShowLoadingScreen(false);
	}
	
}

void USKGameInstance::ShowTitleUI()
{
	ULocalPlayer* LP = GetFirstGamePlayer();
	if (!LP) return;

	APlayerController* PC = LP->GetPlayerController(GetWorld());
	if (!PC) return;

	if (!TitleWidgetClass)
		return;

	TitleWidgetInstance = CreateWidget<UCommonActivatableWidget>(
		PC,
		TitleWidgetClass
	);

	if (!TitleWidgetInstance)
		return;

	TitleWidgetInstance->AddToViewport(1000); // HUD보다 위
	TitleWidgetInstance->ActivateWidget();
}

void USKGameInstance::StartGameFromTitle()
{
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] StartGameFromTitle"));

	UWorld* World = GetWorld();
	if (!World)
		return;

	World->ServerTravel(
	TEXT("/Game/BluePrint/Level/RuinsTutorial?listen"),
	true
	);
}
