// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/EquipMainItemWidget.h"

#include "Component/EquipmentComponent.h"
#include "Components/Image.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Utility/StaticDataSubsystem.h"

void UEquipMainItemWidget::SetItem(const int32 ItemID, UEquipmentComponent* EquipmentComponent)
{
	CurrentEquipMainItem.CurrentItemID = ItemID;

	UWorld* World = GetWorld();
	if (!World) return;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;
	
	UStaticDataSubsystem* SDS = GI->GetSubsystem<UStaticDataSubsystem>();
	if(!SDS) return;

	const FItemData* ItemData = SDS->GetData<FItemData>(CurrentEquipMainItem.CurrentItemID);
	if (!ItemData)
	{
		return;
	}

	USKInventoryItemData* CurrentItemData = nullptr;
	if (ItemData->InventoryItemDataAsset.IsValid())
	{
		CurrentItemData = ItemData->InventoryItemDataAsset.Get();
	}
	else if (!ItemData->InventoryItemDataAsset.ToSoftObjectPath().IsNull())
	{
		CurrentItemData = ItemData->InventoryItemDataAsset.LoadSynchronous();
	}
	
	if (!CurrentItemData)
	{
		if (ItemID == -1)
		{
			ItemIcon->SetBrush(FSlateBrush());
		}
		return;
	}

	if (ItemIcon)
	{
		if (CurrentItemData->ItemIcon && IsValid(CurrentItemData->ItemIcon))
		{
			UE_LOG(LogTemp, Warning, TEXT("SetBrushFromTexture: Valid ItemIcon found. Texture name: %s"), *CurrentItemData->ItemIcon->GetName());
			// 아이콘이 유효하면 텍스처 설정
			ItemIcon->SetBrushFromTexture(CurrentItemData->ItemIcon);
		}
		else
		{
			if (!CurrentItemData->ItemIcon)
			{
				UE_LOG(LogTemp, Warning, TEXT("CurrentItemData->ItemIcon is nullptr."));
			}
			else if (!IsValid(CurrentItemData->ItemIcon))
			{
				UE_LOG(LogTemp, Warning, TEXT("CurrentItemData->ItemIcon is invalid UObject."));
			}
 
			// 아이콘 없으면 브러시 초기화 (없앰)
			ItemIcon->SetBrush(FSlateBrush());
			UE_LOG(LogTemp, Warning, TEXT("SetBrush: ItemIcon brush cleared."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemIcon widget is nullptr."));
	}

	if (CurrentItemData->InventoryType == EInventoryItemType::Equipment)
	{
		USKEquipmentItemData* CurrentEquipData = Cast<USKEquipmentItemData>(CurrentItemData);
		if (!CurrentEquipData) return;

		CurrentEquipMainItem.CurrentEquipInstance = EquipmentComponent->GetEquipment(CurrentEquipData->SlotType)->EquipmentInstance;
	}	
}

void UEquipMainItemWidget::SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType)
{
	CurrentEquipMainItem.CurrentItemID = -1;
	CurrentItemType = ItemType;
	CurrentEquipSlotType = SlotType;
	CurrentEquipMainItem.CurrentEquipInstance = nullptr;
}

void UEquipMainItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnItemHovered();
}

void UEquipMainItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnItemUnhovered();
}

FReply UEquipMainItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// BlueprintCallable로 열어둔 함수 호출도 가능
		OnItemClicked();

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipMainItemWidget::OnItemHovered()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentEquipMainItem.CurrentItemID, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void UEquipMainItemWidget::OnItemUnhovered()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentEquipMainItem.CurrentItemID, false);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}

void UEquipMainItemWidget::OnItemClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FToolTipSwitch Message(CurrentEquipMainItem.CurrentItemID, false);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ToolTipItem, Message);
		}
	}
}
