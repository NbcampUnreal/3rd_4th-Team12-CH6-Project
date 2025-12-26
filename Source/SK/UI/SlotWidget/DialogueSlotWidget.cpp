// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/DialogueSlotWidget.h"

#include "Components/TextBlock.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UDialogueSlotWidget::DialogueSettingMessageReceived(FGameplayTag Channel, const FDialogSendMessage& Message)
{
	StartTyping(Message.MessageText);
}

void UDialogueSlotWidget::DialogueSkipMessageReceived(FGameplayTag Channel, const FDialogSkipMessage& Message)
{
	if (bTyping)
	{
		SkipTyping();
	}
	else
	{
		if (Anim_CloseImage)
		{
			StopAnimation(Anim_CloseImage); // 애니메이션 즉시 중단
		}
		
		if (UWorld* World = GetWorld())
		{
			if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
			{
				// 전송할 메시지 생성
				FSwitchLayoutMessage LayoutMessage(TAG_UI_Layout_InGame, true);

				// 메시지 브로드캐스트 (UI 전환용 채널로)
				MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, LayoutMessage);
			}
		}
	}
}

void UDialogueSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		return;
	}

	DialogueMessageHandle = MessageSubsystem->RegisterListener<FDialogSendMessage>(
		TAG_Message_Channel_NPCDialogue,
		this,
		&UDialogueSlotWidget::DialogueSettingMessageReceived
	);

	DialogueSkipHandle = MessageSubsystem->RegisterListener<FDialogSkipMessage>(
		TAG_Message_Channel_NPCDialogueSkip,
		this,
		&UDialogueSlotWidget::DialogueSkipMessageReceived
	);
}

void UDialogueSlotWidget::NativeDestruct()
{
	if (DialogueMessageHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(DialogueMessageHandle);
		}
	}

	if (DialogueSkipHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(DialogueSkipHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UDialogueSlotWidget::StartTyping(const FString& InText)
{
	FullText = InText;
	CurrentCharIndex = 0;
	bTyping = true;
	DialogueText->SetText(FText::GetEmpty());

	GetWorld()->GetTimerManager().SetTimer(
		TypingTimerHandle,
		this,
		&UDialogueSlotWidget::TypeNextChar,
		TypingInterval,
		true
	);
}

void UDialogueSlotWidget::TypeNextChar()
{
	if (CurrentCharIndex >= FullText.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
		PlayAnimation(
			Anim_CloseImage,
			0.f,
			20,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
		return;
	}

	FString CurrentText = FullText.Left(CurrentCharIndex + 1);
	DialogueText->SetText(FText::FromString(CurrentText));

	CurrentCharIndex++;
}

void UDialogueSlotWidget::SkipTyping()
{
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
	DialogueText->SetText(FText::FromString(FullText));
	bTyping = false;
	PlayAnimation(
		Anim_CloseImage,
		0.f,
		20,
		EUMGSequencePlayMode::Forward,
		1.0f
	);
}
