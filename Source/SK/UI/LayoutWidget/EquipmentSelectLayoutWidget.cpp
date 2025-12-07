// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EquipmentSelectLayoutWidget.h"

#include "Components/Button.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentSelectLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SelectToMainHandle = RegisterUIActionBinding(FBindUIActionArgs(SelectToMainActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelectToMainAction)));

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear(); // 혹시 중복 방지
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

void UEquipmentSelectLayoutWidget::HandleCloseButtonClicked()
{
	HandleSelectToMainAction();
}
