// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/SKConfirmLayouWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void USKConfirmLayouWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::BackHandleAction)));
	MoveLeftHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveLeftData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::MoveLeftHandleAction)));
	MoveRightHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveRightData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::MoveRightHandleAction)));
	SelectionHandle = RegisterUIActionBinding(FBindUIActionArgs(SelectionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::SelectionHandleAction)));
}

void USKConfirmLayouWidget::BackHandleAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Confirm, 5);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void USKConfirmLayouWidget::MoveLeftHandleAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Confirm, 2);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void USKConfirmLayouWidget::MoveRightHandleAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Confirm, 3);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void USKConfirmLayouWidget::SelectionHandleAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Confirm, 4);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}
