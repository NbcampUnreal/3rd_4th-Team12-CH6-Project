// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/EquipmentSelect/EquipSelectItemWidget.h"

#include "EquipSelectListSlotWidget.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameInstance/SKGameInstance.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKBGMSubSystem.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipSelectItemWidget::SetItem(FInventorySlot SettingInventorySlot)
{
	CurrentInventorySlot = SettingInventorySlot;
	
	if (!CachedInventory)
		return;

	USKInventoryItemData* CurrentItemData = CachedInventory->GetItemDataByID(SettingInventorySlot.ItemID);
	if (!CurrentItemData)
	{
		if (ItemNameText)
		{
			ItemNameText->SetText(FText::GetEmpty());
		}
		if (ItemQuantityText)
		{
			ItemQuantityText->SetText(FText::GetEmpty());
		}
		if (ItemIcon)
		{
			// 아이콘 없으면 브러시 초기화 (없앰)
			ItemIcon->SetBrush(FSlateBrush());
		}
	}
	else
	{
		if (ItemNameText)
		{
			ItemNameText->SetText(FText::FromName(CurrentItemData->ItemID));
		}
		if (ItemQuantityText)
		{
			ItemQuantityText->SetText(FText::AsNumber(SettingInventorySlot.Count));
		}
		if (ItemIcon)
		{
			// 아이콘 없으면 브러시 초기화 (없앰)
			ItemIcon->SetBrushFromTexture(CurrentItemData->ItemIcon);
		}
	}
}

void UEquipSelectItemWidget::SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, int32 QuickSlotNum)
{
	CurrentItemType = ItemType;
	CurrentEquipSlotType = SlotType;
	CurrentQuickSlotNum = QuickSlotNum;
}

void UEquipSelectItemWidget::SendComponent(UQuickSlotComponent* QSComponent,
	UEquipmentComponent* EquipComponent,
	UInventoryComponent* InventoryComponent)
{
	CachedInventory = InventoryComponent;
	CachedEquipment = EquipComponent;
	CachedQuickSlot = QSComponent;
}

void UEquipSelectItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	SendHoverMessage(true);
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

void UEquipSelectItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	SendHoverMessage(false);
	HoverImageVisible(false);
}

FReply UEquipSelectItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// BlueprintCallable로 열어둔 함수 호출도 가능
		OnItemLeftClicked();
		//PlayUISound(SelectSound);

		UWorld* World = GetWorld();
		USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());

		if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
		{
			BGM->PlayUISoundByTag(TAG_GameplayCue_Sound_UI_SelectSound);
		}
		
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// BlueprintCallable로 열어둔 함수 호출도 가능
		OnItemRightClicked();
		// PlayUISound(UnSelectSound);
		UWorld* World = GetWorld();
		USKGameInstance* SKGI = Cast<USKGameInstance>(World->GetGameInstance());

		if (USKBGMSubSystem* BGM = SKGI->GetSubsystem<USKBGMSubSystem>())
		{
			BGM->PlayUISoundByTag(TAG_GameplayCue_Sound_UI_UnSelectSound);
		}
		return FReply::Handled();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipSelectItemWidget::SendHoverMessage(bool bHover)
{
	if (CurrentInventorySlot.ItemID == -1) return;
	
	if (UWorld* World = GetWorld())
	{
		if (auto* MSG = USKGameplayMessageSubsystem::Get(World))
		{
			FToolTipSwitch Msg(CurrentInventorySlot.ItemID, bHover);
			MSG->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Msg);
		}
	}
}


void UEquipSelectItemWidget::OnItemLeftClicked()
{
	APlayerState* OwnerPS = GetOwningPlayerState();
	if (!OwnerPS)
	{
		return;
	}

	if (CurrentItemType == EInventoryItemType::Consumable)
	{
		if (!CachedQuickSlot)
			return;

		if (OwnerPS->HasAuthority())
		{
			CachedQuickSlot->SetQuickSlot(CurrentQuickSlotNum, CurrentInventorySlot.ItemID);
		}
		else
		{
			CachedQuickSlot->ServerSetQuickSlot(CurrentQuickSlotNum, CurrentInventorySlot.ItemID);
		}	
	}
	else if (CurrentItemType == EInventoryItemType::Equipment)
	{
		if (!CachedEquipment)
			return;
		
		if (OwnerPS->HasAuthority())
		{
			CachedEquipment->EquipItem(CurrentInventorySlot.UniqueID, CurrentInventorySlot.ItemID);
		}
		else
		{
			CachedEquipment->ServerEquipItem(CurrentInventorySlot.UniqueID, CurrentInventorySlot.ItemID);
		}
	}

	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_EquipmentMain, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);
		}
	}
	
}

void UEquipSelectItemWidget::OnItemRightClicked()
{
	APlayerState* OwnerPS = GetOwningPlayerState();
	if (!OwnerPS)
	{
		return;
	}
		

	if (CurrentItemType == EInventoryItemType::Consumable)
	{
		if (!CachedQuickSlot)
			return;

		if (OwnerPS->HasAuthority())
		{
			CachedQuickSlot->ClearQuickSlot(CurrentQuickSlotNum);
		}
		else
		{
			CachedQuickSlot->ServerClearQuickSlot(CurrentQuickSlotNum);
		}	
	}
	else if (CurrentItemType == EInventoryItemType::Equipment)
	{
		if (!CachedEquipment)
			return;
		
		if (OwnerPS->HasAuthority())
		{
			CachedEquipment->UnequipItem(CurrentEquipSlotType);
		}
		else
		{
			CachedEquipment->ServerUnequipItem(CurrentEquipSlotType);
		}
	}

	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_EquipmentMain, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);
		}
	}
}

void UEquipSelectItemWidget::HoverImageVisible(bool bVisible)
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

void UEquipSelectItemWidget::PlayUISound(USoundBase* InSound)
{
	if (!InSound) return;

	UGameplayStatics::PlaySound2D(this, InSound);
}
