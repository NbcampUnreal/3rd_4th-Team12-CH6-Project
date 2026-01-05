// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"

#include "InventoryListSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameInstance/SKGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKBGMSubSystem.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UInventoryItemWidget::SetItem(const FInventoryItemForWidget& NewItem)
{
	CurrentItem = NewItem;

	if (ItemNameText)
	{
		if (NewItem.ItemName == "NO_Item")
		{
			ItemNameText->SetText(FText::GetEmpty());
		}
		else
		{
			ItemNameText->SetText(FText::FromName(NewItem.ItemName));
		}
	}
		

	if (ItemQuantityText)
	{
		if (NewItem.Quantity == 0)
		{
			ItemQuantityText->SetText(FText::GetEmpty());
		}
		else
		{
			ItemQuantityText->SetText(FText::AsNumber(NewItem.Quantity));
		}
	}

	if (ItemIcon)
	{
		if (NewItem.Icon && IsValid(NewItem.Icon))
		{
			// 아이콘이 유효하면 텍스처 설정
			ItemIcon->SetBrushFromTexture(NewItem.Icon);
		}
		else
		{
			// 아이콘 없으면 브러시 초기화 (없앰)
			ItemIcon->SetBrush(FSlateBrush());
		}
	}
}

void UInventoryItemWidget::HoverImageVisible(bool bVisible)
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

void UInventoryItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnItemHovered();
	HoverImageVisible(true);
	
	ParentWidget->NotifyIndex(WidgetIndex);
	// PlayUISound(HoverSound);
	UWorld* World = GetWorld();
	if (!World)
		return;

	USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGI)
		return;

	if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->PlayUISoundByTag(TAG_GameplayCue_Sound_UI_HoverSound);
	}
}

void UInventoryItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnItemUnhovered();
	HoverImageVisible(false);
}

void UInventoryItemWidget::OnItemHovered()
{
	if (CurrentItem.ItemID == -1)
	{
		return;
	}
	
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentItem.ItemID, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void UInventoryItemWidget::OnItemUnhovered()
{
	if (CurrentItem.ItemID == -1)
	{
		return;
	}
	
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentItem.ItemID, false);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void UInventoryItemWidget::PlayUISound(USoundBase* InSound)
{
	if (!InSound) return;

	UGameplayStatics::PlaySound2D(this, InSound);
}
