// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/SlectItemWidget.h"

#include "Component/EquipmentComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void USlectItemWidget::SetItem(FInventorySlot InvenSlot)
{
	CurrentInventorySlot = InvenSlot;

	UInventoryComponent* Invnetory = GetOwningPlayerState()->FindComponentByClass<UInventoryComponent>();

	if (!Invnetory)
		return;

	USKInventoryItemData* CurrentItemData = Invnetory->GetItemDataByID(InvenSlot.ItemID);
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
			ItemQuantityText->SetText(FText::AsNumber(InvenSlot.Count));
		}
		if (ItemIcon)
		{
			// 아이콘 없으면 브러시 초기화 (없앰)
			ItemIcon->SetBrushFromTexture(CurrentItemData->ItemIcon);
		}
	}
	
}

void USlectItemWidget::SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, int32 QuickSlotNum)
{
	CurrentItemType = ItemType;
	CurrentEquipSlotType = SlotType;
	CurrentQuickSlotNum = QuickSlotNum;

	const UEnum* ItemTypeEnum = StaticEnum<EInventoryItemType>();
	const UEnum* SlotTypeEnum = StaticEnum<EEquipmentSlotType>();
 
	FString ItemTypeString = ItemTypeEnum ? ItemTypeEnum->GetNameStringByValue((int64)ItemType) : FString::Printf(TEXT("Unknown(%d)"), (int32)ItemType);
	FString SlotTypeString = SlotTypeEnum ? SlotTypeEnum->GetNameStringByValue((int64)SlotType) : FString::Printf(TEXT("Unknown(%d)"), (int32)SlotType);
 
	// 로그 출력: Output Log
	UE_LOG(LogTemp, Warning, TEXT("SettingSlot called! ItemType: %s, SlotType: %s, QuickSlotNum: %d"), *ItemTypeString, *SlotTypeString, QuickSlotNum);
	UE_LOG(LogTemp, Warning, TEXT("Updated States -> CurrentItemType: %s, CurrentEquipSlotType: %s, CurrentQuickSlotNum: %d"), *ItemTypeString, *SlotTypeString, CurrentQuickSlotNum);
}

void USlectItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnItemHovered();
}

void USlectItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnItemUnhovered();
}

FReply USlectItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// BlueprintCallable로 열어둔 함수 호출도 가능
		OnItemLeftClicked();

		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// BlueprintCallable로 열어둔 함수 호출도 가능
		OnItemRightClicked();

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void USlectItemWidget::OnItemHovered()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentInventorySlot.ItemID, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void USlectItemWidget::OnItemUnhovered()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentInventorySlot.ItemID, false);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void USlectItemWidget::OnItemLeftClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ItemClicked"));
	APlayerState* OwnerPS = GetOwningPlayerState();
	if (!OwnerPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked OwnerPS No"));
		return;
	}
		

	if (CurrentItemType == EInventoryItemType::Consumable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable"));
		UQuickSlotComponent* CurrentQuickSlot = OwnerPS->FindComponentByClass<UQuickSlotComponent>();
		if (!CurrentQuickSlot)
			return;

		if (OwnerPS->HasAuthority())
		{
			CurrentQuickSlot->SetQuickSlot(CurrentQuickSlotNum, CurrentInventorySlot.ItemID);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable ServerSetting"));
		}
		else
		{
			CurrentQuickSlot->ServerSetQuickSlot(CurrentQuickSlotNum, CurrentInventorySlot.ItemID);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable ClientSetting"));
		}	
	}
	else if (CurrentItemType == EInventoryItemType::Equipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment"));
		UEquipmentComponent* CurrentEquipment = OwnerPS->GetComponentByClass<UEquipmentComponent>();
		if (!CurrentEquipment)
			return;
		
		if (OwnerPS->HasAuthority())
		{
			CurrentEquipment->EquipItem(CurrentInventorySlot.UniqueID, CurrentInventorySlot.ItemID);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment ServerSetting"));
		}
		else
		{
			CurrentEquipment->ServerEquipItem(CurrentInventorySlot.UniqueID, CurrentInventorySlot.ItemID);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment ClientSetting"));
		}
	}
}

void USlectItemWidget::OnItemRightClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ItemClicked"));
	APlayerState* OwnerPS = GetOwningPlayerState();
	if (!OwnerPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked OwnerPS No"));
		return;
	}
		

	if (CurrentItemType == EInventoryItemType::Consumable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable"));
		UQuickSlotComponent* CurrentQuickSlot = OwnerPS->FindComponentByClass<UQuickSlotComponent>();
		if (!CurrentQuickSlot)
			return;

		if (OwnerPS->HasAuthority())
		{
			CurrentQuickSlot->ClearQuickSlot(CurrentQuickSlotNum);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable ServerSetting"));
		}
		else
		{
			CurrentQuickSlot->ServerClearQuickSlot(CurrentQuickSlotNum);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Consumable ClientSetting"));
		}	
	}
	else if (CurrentItemType == EInventoryItemType::Equipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment"));
		UEquipmentComponent* CurrentEquipment = OwnerPS->GetComponentByClass<UEquipmentComponent>();
		if (!CurrentEquipment)
			return;
		
		if (OwnerPS->HasAuthority())
		{
			CurrentEquipment->UnequipItem(CurrentEquipSlotType);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment ServerSetting"));
		}
		else
		{
			CurrentEquipment->ServerUnequipItem(CurrentEquipSlotType);
			UE_LOG(LogTemp, Warning, TEXT("ItemClicked: Equipment ClientSetting"));
		}
	}
}
