// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GameExitRequestSlotWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/SKNativeGameplayTags.h"

void UGameExitRequestSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UGameExitRequestSlotWidget::OnContinueClicked);
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
}

void UGameExitRequestSlotWidget::NativeDestruct()
{
	if (ConfirmResponseHandle.IsValid())
	{
		ConfirmResponseHandle.Unregister();
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

			FSlotVisibilityMessage VisibilityMessage;
			VisibilityMessage.LayoutTag = TAG_UI_Layout_InGame;
			VisibilityMessage.SlotTags.AddTag(TAG_UI_Slot_CharacterStatus);
			VisibilityMessage.bVisible = true;
			VisibilityMessage.VisibleDuration = 3.0f;

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible,	VisibilityMessage);
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

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *Message.SlotTag.ToString());
		}
	}
}



void UGameExitRequestSlotWidget::OnConfirmResponseMessageReceived(FGameplayTag Channel,
	const FConfirmResponseMessage& Message)
{
	if (Message.SlotTag != TAG_UI_Slot_GameExitRequest)
		return;

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
