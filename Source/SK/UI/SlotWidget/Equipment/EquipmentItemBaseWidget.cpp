// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Equipment/EquipmentItemBaseWidget.h"

#include "EquipMainListSlotWidget.h"
#include "Components/Image.h"
#include "GameInstance/SKGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKBGMSubSystem.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentItemBaseWidget::SetIndex(int32 Index)
{
	CurrentIndex = Index;
}

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

	ParentWidget->NotifyIndex(CurrentIndex);
	// PlayUISound(HoverSound);
	UWorld* World = GetWorld();
	if (!World)
		return;

	USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGI)
		return;

	if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
	{
		//BGM->PlayUISoundByTag(TAG_GameplayCue_Sound_UI_HoverSound);
	}
}

void UEquipmentItemBaseWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SendHoverMessage(false);
}

void UEquipmentItemBaseWidget::HoverImageVisible(bool bVisible)
{
	if (bVisible)
	{
		if (HoverImage)
		{
			HoverImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (HoverImage)
		{
			HoverImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UEquipmentItemBaseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnClicked();
	// PlayUISound(ClickSound);

	UWorld* World = GetWorld();
	USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());

	if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->PlayUISoundByTag(TAG_GameplayCue_Sound_UI_ClickSound);
	}
	
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

void UEquipmentItemBaseWidget::PlayUISound(USoundBase* InSound)
{
	if (!InSound) return;

	UGameplayStatics::PlaySound2D(this, InSound);
}
