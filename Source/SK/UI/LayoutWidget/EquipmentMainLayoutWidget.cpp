// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EquipmentMainLayoutWidget.h"

#include "Components/Button.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentMainLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
 
	EquipMainToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(EquipMainToInGameData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEquipMainToInGameAction)));
	EquipMainToInGameHandle2 = RegisterUIActionBinding(FBindUIActionArgs(EquipMainToInGameData2, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEquipMainToInGameAction)));
	MoveUpHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveUpData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUpAction)));
	MoveDownHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveDownData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDownAction)));
	MoveLeftHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveLeftData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeftAction)));
	MoveRightHandle = RegisterUIActionBinding(FBindUIActionArgs(MoveRightData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRightAction)));
	SelectionHandle = RegisterUIActionBinding(FBindUIActionArgs(SelectionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectionAction)));

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear(); // 혹시 중복 방지
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
}

void UEquipmentMainLayoutWidget::HandleEquipMainToInGameAction()
{

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}
	
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

void UEquipmentMainLayoutWidget::HandleMoveUpAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Equipmentmain, 0);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentMainLayoutWidget::HandleMoveDownAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Equipmentmain, 1);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentMainLayoutWidget::HandleMoveLeftAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Equipmentmain, 2);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentMainLayoutWidget::HandleMoveRightAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Equipmentmain, 3);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentMainLayoutWidget::HandleSelectionAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FUIInteractionMoveMessage Message(EUIMessageType::Equipmentmain, 4);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_UIInteraction, Message);
		}
	}
}

void UEquipmentMainLayoutWidget::HandleCloseButtonClicked()
{
	HandleEquipMainToInGameAction();
}
