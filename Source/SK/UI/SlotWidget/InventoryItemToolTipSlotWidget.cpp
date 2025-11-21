// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/InventoryItemToolTipSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameData/StaticData/ItemDataTable.h"
#include "Item/Inventory/Data/SKConsumableItemData.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "Item/Inventory/Data/SKMiscItemData.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Utility/StaticDataSubsystem.h"

void UInventoryItemToolTipSlotWidget::SetItemData(USKInventoryItemData* ItemData)
{
	if (!ItemData)
	{
		// 초기화
		if (ItemIcon) ItemIcon->SetBrush(FSlateBrush());
		if (ItemNameText) ItemNameText->SetText(FText::GetEmpty());
		if (ItemDescriptionText) ItemDescriptionText->SetText(FText::GetEmpty());
		if (ItemRarityText) ItemRarityText->SetText(FText::GetEmpty());
		if (ItemTypeText) ItemTypeText->SetText(FText::GetEmpty());
		if (ItemStatText) ItemStatText->SetText(FText::GetEmpty());
		return;
	}

	if (ItemIcon)
	{
		if (ItemData->ItemIcon && ItemData->ItemIcon->IsValidLowLevel())
		{
			ItemIcon->SetBrushFromTexture(ItemData->ItemIcon);
		}
		else
		{
			ItemIcon->SetBrush(FSlateBrush());
		}
	}
 
	if (ItemNameText)
	{
		ItemNameText->SetText(ItemData->ItemName);
	}
 
	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(ItemData->ItemDescription);
	}
 
	if (ItemRarityText)
	{
		FString RarityName = StaticEnum<EItemRarity>()->GetNameStringByValue((int64)ItemData->ItemRarity);
		ItemRarityText->SetText(FText::FromString(RarityName));
	}
 
	if (ItemTypeText)
	{
		FString TypeName = StaticEnum<EInventoryItemType>()->GetNameStringByValue((int64)ItemData->InventoryType);
		ItemTypeText->SetText(FText::FromString(TypeName));
	}
 
	UpdateAdditionalStats(ItemData);

	SetVisibility(ESlateVisibility::Visible);
}

void UInventoryItemToolTipSlotWidget::OnToolTipSwitchMessageReceived(FGameplayTag Channel,
	const FToolTipSwitch& Message)
{
	if (!Message.bEnter || !Message.MessageItemID)
	{
		if (ItemIcon) ItemIcon->SetBrush(FSlateBrush());
		if (ItemNameText) ItemNameText->SetText(FText::GetEmpty());
		if (ItemDescriptionText) ItemDescriptionText->SetText(FText::GetEmpty());
		if (ItemRarityText) ItemRarityText->SetText(FText::GetEmpty());
		if (ItemTypeText) ItemTypeText->SetText(FText::GetEmpty());
		if (ItemStatText) ItemStatText->SetText(FText::GetEmpty());
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;
	
	UStaticDataSubsystem* SDS = GI->GetSubsystem<UStaticDataSubsystem>();
	if(!SDS) return;

	const FItemData* ItemData = SDS->GetData<FItemData>(Message.MessageItemID);
	if (!ItemData)
	{
		return;
	}
	
	if (ItemData->InventoryItemDataAsset.IsValid())
	{
		SetItemData(ItemData->InventoryItemDataAsset.Get());
	}
	else if (!ItemData->InventoryItemDataAsset.ToSoftObjectPath().IsNull())
	{
		SetItemData(ItemData->InventoryItemDataAsset.LoadSynchronous());
	}
}

void UInventoryItemToolTipSlotWidget::UpdateAdditionalStats(USKInventoryItemData* ItemData)
{
	if (!ItemStatText)
		return;
 
	FString StatString;
 
	if (USKEquipmentItemData* EquipData = Cast<USKEquipmentItemData>(ItemData))
	{
		StatString = FString::Printf(TEXT("공격력: %.0f\n방어력: %.0f"), EquipData->AttackPower, EquipData->DefensePower);
	}
	else if (USKConsumableItemData* ConsumableData = Cast<USKConsumableItemData>(ItemData))
	{
		StatString = FString::Printf(TEXT("회복량: %d\n지속시간: %.1f초"), ConsumableData->Amount, ConsumableData->Duration);
	}
	else if (USKMiscItemData* MiscData = Cast<USKMiscItemData>(ItemData))
	{
		TArray<FString> Flags;
		if (MiscData->bIsQuestItem) Flags.Add(TEXT("퀘스트 아이템"));
		if (MiscData->bIsCurrency) Flags.Add(TEXT("통화 아이템"));
		if (MiscData->bIsKeyItem) Flags.Add(TEXT("열쇠 아이템"));
		if (MiscData->bIsMaterial) Flags.Add(TEXT("재료 아이템"));
		StatString = FString::Join(Flags, TEXT(", "));
	}
	else
	{
		StatString = TEXT("");
	}
 
	ItemStatText->SetText(FText::FromString(StatString));
}

void UInventoryItemToolTipSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	// 메시지 구독
	ToolTipSwitchHandle = MessageSubsystem->RegisterListener<FToolTipSwitch>(
		TAG_Message_Channel_ToolTipItem,
		this,
		&UInventoryItemToolTipSlotWidget::OnToolTipSwitchMessageReceived
	);
}

