// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Equipment/EquipmentItemBaseWidget.h"

#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentItemBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquipmentItemBaseWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UEquipmentItemBaseWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	SendHoverMessage(true);
}

void UEquipmentItemBaseWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SendHoverMessage(false);
}

FReply UEquipmentItemBaseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnClicked();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

//ToDO 툴팁의 내용이 |데에터 에셋 -> 장비인스턴스|로 바뀔 경우 메시지 분리 혹은 분기 처리 필요
void UEquipmentItemBaseWidget::SendHoverMessage(bool bHover) const
{
	if (CurrentItemID == -1) return;
	
	if (UWorld* World = GetWorld())
	{
		if (auto* MSG = USKGameplayMessageSubsystem::Get(World))
		{
			FToolTipSwitch Msg(CurrentItemID, bHover);
			MSG->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Msg);
		}
	}
}
