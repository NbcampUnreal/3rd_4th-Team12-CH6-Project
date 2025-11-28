// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Equipment/EquipmentItemEquipWidget.h"

#include "Component/InventoryComponent.h"
#include "Components/Image.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentItemEquipWidget::SettingItem(int32 ItemID, UEquipmentInstance* ItemInstance)
{
	CurrentItemID = ItemID;
	CurrentEquipInstance = ItemInstance;

	if (ItemID == -1 || !CurrentInventoryComponent)
	{
		ItemIcon->SetBrush(FSlateBrush());
		return;
	}

	USKInventoryItemData* CurrentItemData = CurrentInventoryComponent->GetItemDataByID(ItemID);
	if (!CurrentItemData)
		return;

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(CurrentItemData->ItemIcon);
	}
}

void UEquipmentItemEquipWidget::SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, UInventoryComponent* InventoryComponent)
{
	CurrentItemType = ItemType;
	CurrentSlotType = SlotType;
	CurrentInventoryComponent = InventoryComponent;
}

void UEquipmentItemEquipWidget::OnClicked()
{
	Super::OnClicked();
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			FSwitchLayoutMessage SwitchMessage;
			
			SwitchMessage.bVisible = true;
			SwitchMessage.LayoutTag = TAG_UI_Layout_EquipmentSelect;

			FItemSwitchMessage SwitchSettingMessage;
			
			SwitchSettingMessage.EquipmentType = CurrentSlotType;
			SwitchSettingMessage.ItemType = CurrentItemType;
			SwitchSettingMessage.QuickSlotNumber = 0;
			
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, SwitchMessage);
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ItemSwitchSelect, SwitchSettingMessage);
		}
	}
}
