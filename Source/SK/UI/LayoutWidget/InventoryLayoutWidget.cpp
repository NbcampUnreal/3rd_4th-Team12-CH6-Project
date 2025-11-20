// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/InventoryLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UInventoryLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InventoryToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(InventoryToInGameData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleInventoryToInGameAction)));
}

void UInventoryLayoutWidget::HandleInventoryToInGameAction()
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
