// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GameExitRequestSlotWidget.h"

#include "Components/Button.h"
#include "GameInstance/SKGameInstance.h"
#include "GameMode/MatchState/DungeonMatchState.h"
#include "GameState/DungeonGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/SKNativeGameplayTags.h"

void UGameExitRequestSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UGameExitRequestSlotWidget::OnContinueClicked);
	}

	if (DungeonExitButton)
	{
		DungeonExitButton->OnClicked.AddDynamic(this, &UGameExitRequestSlotWidget::OnDungeonExitClicked);
	}

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UGameExitRequestSlotWidget::OnOptionClicked);
	}
	
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UGameExitRequestSlotWidget::OnExitClicked);
	}
	
	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	ConfirmResponseHandle = MessageSubsystem->RegisterListener<FConfirmResponseMessage>(
		TAG_Message_Channel_ConfirmResponse,
		this,
		&UGameExitRequestSlotWidget::OnConfirmResponseMessageReceived
	);

	bool bInDungeon = false;
	
	if (ADungeonGameState* GS = GetWorld()->GetGameState<ADungeonGameState>())
	{
		bInDungeon = GS->DungeonState != EDungeonMatchState::None;
	}

	// 버튼 Visible 토글
	DungeonExitButton->SetVisibility(bInDungeon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UGameExitRequestSlotWidget::NativeDestruct()
{
	if (ConfirmResponseHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(ConfirmResponseHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UGameExitRequestSlotWidget::OnContinueClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_InGame, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *Message.LayoutTag.ToString());
		}
	}	
}

void UGameExitRequestSlotWidget::OnDungeonExitClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmUIMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = TAG_UI_Slot_GameExitRequest;
			Message.Title = FText::FromString(TEXT("던전 퇴장"));
			Message.Message = FText::FromString(TEXT("정말로 던전에서 마을로 이동하시겠습니까?"));
			Message.ConfirmText = FText::FromString(TEXT("퇴장"));
			Message.CancelText = FText::FromString(TEXT("취소"));
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_RequestConfirm, Message);
			
			RequestButton = 2;
		}
	}
}

void UGameExitRequestSlotWidget::OnExitClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmUIMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = TAG_UI_Slot_GameExitRequest;
			Message.Title = FText::FromString(TEXT("게임 종료"));
			Message.Message = FText::FromString(TEXT("정말로 게임을 종료하시겠습니까?"));
			Message.ConfirmText = FText::FromString(TEXT("종료"));
			Message.CancelText = FText::FromString(TEXT("취소"));
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_RequestConfirm, Message);
			
			RequestButton = 1;
		}
	}
}

void UGameExitRequestSlotWidget::OnOptionClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			FSwitchLayoutMessage Message(TAG_UI_Layout_Option,true );

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout,	Message);
		}
	}
}


void UGameExitRequestSlotWidget::OnConfirmResponseMessageReceived(FGameplayTag Channel,
                                                                  const FConfirmResponseMessage& Message)
{
	if (Message.SlotTag != TAG_UI_Slot_GameExitRequest)
	{
		RequestButton = 0;
		return;
	}
		

	if (RequestButton == 1)
	{
		if (Message.bAccepted)
		{
			APlayerController* PC = GetOwningPlayer();
			if (PC)
			{
				UKismetSystemLibrary::QuitGame(
					this,       // WorldContextObject
					PC,         // PlayerController
					EQuitPreference::Quit,  // 종료 옵션
					true        // bIgnorePlatformRestrictions
				);
			}
		}
	}
	else if (RequestButton == 2)
	{
		if (Message.bAccepted)
		{
			if (UWorld* World = GetWorld())
			{
				const ENetMode NetMode = World->GetNetMode();

				if (NetMode == NM_Client)
				{
					// 클라이언트라면 세션 종료 후 로컬 레벨로 이동
					if (USKGameInstance* GI = Cast<USKGameInstance>(UGameplayStatics::GetGameInstance(World)))
					{
						GI->LeaveSession();
					}
				}
				else if (NetMode == NM_ListenServer || NetMode == NM_DedicatedServer || NetMode == NM_Standalone)
				{
					// 호스트라면 ServerTravel 호출
					if (USKGameInstance* GI = Cast<USKGameInstance>(UGameplayStatics::GetGameInstance(World)))
					{
						GI->TravelToTown();
					}
				}
			}
		}
	}

	RequestButton = 0;
}
