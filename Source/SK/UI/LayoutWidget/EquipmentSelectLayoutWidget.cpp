// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EquipmentSelectLayoutWidget.h"

#include "Components/Button.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentSelectLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SelectToMainHandle = RegisterUIActionBinding(FBindUIActionArgs(SelectToMainActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectToMainAction)));
	MoveUpHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveUpData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUpAction)));
	MoveDownHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveDownData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDownAction)));
	MoveLeftHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveLeftData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeftAction)));
	MoveRightHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveRightData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRightAction)));
	SelectionHandle = RegisterUIActionBinding(FBindUIActionArgs(SelectionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectionAction)));

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear();
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
}

void UEquipmentSelectLayoutWidget::HandleSelectToMainAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_EquipmentMain, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *Message.LayoutTag.ToString());
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleMoveUpAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::EquipmentSelect, 0);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleMoveDownAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::EquipmentSelect, 1);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleMoveLeftAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::EquipmentSelect, 2);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleMoveRightAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::EquipmentSelect, 3);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleSelectionAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::EquipmentSelect, 4);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentSelectLayoutWidget::HandleCloseButtonClicked()
{
	HandleSelectToMainAction();
}
