// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/BonfireMenuLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UBonfireMenuLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	MenuToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuToInGameData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuToInGameAction)));
	MenuUPHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuUPData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMeneUpAction)));
	MenuDownHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuDownData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuDownAction)));
	MenuSelectHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuSelectData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuSelectAction)));
	
}

void UBonfireMenuLayoutWidget::HandleMenuToInGameAction()
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

void UBonfireMenuLayoutWidget::HandleMeneUpAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::BonfireMove, 0);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UBonfireMenuLayoutWidget::HandleMenuDownAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::BonfireMove, 1);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UBonfireMenuLayoutWidget::HandleMenuSelectAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::BonfireMove, 4);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

