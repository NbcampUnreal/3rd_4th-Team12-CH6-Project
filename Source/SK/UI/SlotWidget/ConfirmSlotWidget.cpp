// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ConfirmSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Utility/SKUIManagerSubSystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UConfirmSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleConfirmClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleCancelClicked);
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

	RequestConfirmHandle = MessageSubsystem->RegisterListener<FConfirmUIMessage>(
		TAG_Message_Channel_RequestConfirm,
		this,
		&UConfirmSlotWidget::OnRequestConfirmMessageReceived
	);
}

void UConfirmSlotWidget::NativeDestruct()
{
	if (RequestConfirmHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(RequestConfirmHandle);
		}
	}
	Super::NativeDestruct();
}

void UConfirmSlotWidget::HandleConfirmClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmResponseMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = RequestSlotTag;
			Message.bAccepted = true;
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ConfirmResponse, Message);
		}
	}

	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::HandleCancelClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmResponseMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = RequestSlotTag;
			Message.bAccepted = false;
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ConfirmResponse, Message);
		}
	}

	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::OnRequestConfirmMessageReceived(FGameplayTag Channel, const FConfirmUIMessage& Message)
{	
	if (TitleText)
	{
		TitleText->SetText(Message.Title);
	}

	if (MessageText)
	{
		MessageText->SetText(Message.Message);
	}

	if (ConfirmButtonText)
	{
		ConfirmButtonText->SetText(Message.ConfirmText);
	}

	if (CancelButtonText)
	{
		CancelButtonText->SetText(Message.CancelText);
	}

	RequestSlotTag = Message.SlotTag;
}

