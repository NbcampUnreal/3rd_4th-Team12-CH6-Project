// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
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

void UInventoryItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnItemHovered();
	PlayUISound(HoverSound);
}

void UInventoryItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnItemUnhovered();
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
