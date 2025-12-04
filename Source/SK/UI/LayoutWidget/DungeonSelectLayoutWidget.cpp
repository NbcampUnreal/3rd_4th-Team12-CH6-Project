// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/DungeonSelectLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UDungeonSelectLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(ToInGameActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToInGameAction)));
}

void UDungeonSelectLayoutWidget::HandleToInGameAction()
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
