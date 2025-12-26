// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/DialogueLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UDialogueLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(ToInGameDataEscape, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToInGameAction)));
	ToInGameClickHandle = RegisterUIActionBinding(FBindUIActionArgs(ToInGameDataClick, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToInGameClickAction)));
}

void UDialogueLayoutWidget::HandleToInGameAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_InGame, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);
		}
	}
}

void UDialogueLayoutWidget::HandleToInGameClickAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			FDialogSkipMessage Message(1);
			
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_NPCDialogueSkip, Message);
		}
	}
}
