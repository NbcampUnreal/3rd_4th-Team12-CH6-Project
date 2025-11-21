// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/EquipSelectListSlotWidget.h"

#include "SlectItemWidget.h"
#include "Component/InventoryComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipSelectListSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryCachedInventory();

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
	ItemSwitchHandle = MessageSubsystem->RegisterListener<FItemSwitchMessage>(
		TAG_Message_Channel_ItemSwitchSelect,
		this,
		&UEquipSelectListSlotWidget::OnItemSwitchMessageReceived
	);
}

void UEquipSelectListSlotWidget::TryCachedInventory()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS)
	{
		// 다음 틱에 다시 시도
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedInventory(); });
		return;
	}

	CachedInventory = PS->FindComponentByClass<UInventoryComponent>();
	
	if (!CachedInventory)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedInventory(); });
		return;
	}
}

void UEquipSelectListSlotWidget::OnItemSwitchMessageReceived(FGameplayTag Channel, const FItemSwitchMessage& Message)
{
	CurrentEquipmentSlotType = Message.EquipmentType;
	CurrentItemType = Message.ItemType;
	CurrentQuickSlotNumber = Message.QuickSlotNumber;
	RefreshInventory();
}

void UEquipSelectListSlotWidget::RefreshInventory()
{
	if (!InventoryScroll || !CachedInventory || !ItemWidgetClass) return;
 
	InventoryScroll->ClearChildren();
 
	// 1. RootBox 생성 후 ScrollBox에 추가
	UVerticalBox* RootBox = NewObject<UVerticalBox>(this);
	InventoryScroll->AddChild(RootBox);
 
	// ScrollBox에 추가된 RootBox 슬롯 크기 Automatic으로 변경
	if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(RootBox->Slot))
	{
		ScrollSlot->SetPadding(FMargin(0.f));
		ScrollSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		ScrollSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	}
 
	// 인벤토리 아이템 수 계산

	TArray<FInventorySlot> CachedInventoryArray = CachedInventory->GetItemsByType(CurrentItemType);
	
	if (CurrentItemType == EInventoryItemType::Equipment)
	{
		TArray<FInventorySlot> EquipArray;
		for (const FInventorySlot& TempSlot : CachedInventoryArray)
		{
			USKEquipmentItemData* EquipData = Cast<USKEquipmentItemData>(CachedInventory->GetItemDataByID(TempSlot.ItemID));
			if (EquipData && EquipData->SlotType == CurrentEquipmentSlotType)
			{
				EquipArray.Add(TempSlot);
			}
		}
		CachedInventoryArray = EquipArray;
	}
	
    int32 ItemCount = CachedInventoryArray.Num();
    int32 TotalSlots = FMath::Max(MinSlotCount, FMath::CeilToInt(float(ItemCount) / ItemsPerRow) * ItemsPerRow);
 
    // 아이템 위젯 풀 확보
    while (ItemWidgetPool.Num() < TotalSlots)
    {
        USlectItemWidget* NewWidget = CreateWidget<USlectItemWidget>(this, ItemWidgetClass);
        NewWidget->SetVisibility(ESlateVisibility::Visible);
        ItemWidgetPool.Add(NewWidget);
    }
 
    UHorizontalBox* CurrentRow = nullptr;
    int32 Col = 0;
    for (int32 i = 0; i < TotalSlots; ++i)
    {
        if (Col == 0)
        {
            CurrentRow = NewObject<UHorizontalBox>(this);
            UVerticalBoxSlot* RowSlot = RootBox->AddChildToVerticalBox(CurrentRow);
 
            // CurrentRow를 감싼 VerticalBoxSlot 크기 Automatic으로 변경
            if (RowSlot)
            {
                RowSlot->SetPadding(FMargin(5.f));
                RowSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
                RowSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
                RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            }
        }
 
        USlectItemWidget* ItemWidget = ItemWidgetPool[i];
 
        if (i < ItemCount)
        {
            ItemWidget->SetItem(CachedInventoryArray[i]);
        	ItemWidget->SettingSlot(CurrentItemType, CurrentEquipmentSlotType, CurrentQuickSlotNumber);
            ItemWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            FInventorySlot EmptyItem;
            EmptyItem.ItemID = -1;
            EmptyItem.Count = 0;
            EmptyItem.UniqueID = FGuid::NewGuid();
            ItemWidget->SetItem(EmptyItem);
            ItemWidget->SetVisibility(ESlateVisibility::Visible);
        }
 
        UHorizontalBoxSlot* ItemSlot = CurrentRow->AddChildToHorizontalBox(ItemWidget);
        if (ItemSlot)
        {
            ItemSlot->SetPadding(FMargin(5.f));
            ItemSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
            ItemSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
            ItemSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        }
 
        Col++;
        if (Col >= ItemsPerRow)
            Col = 0;
    }
	
	for (int32 i = TotalSlots; i < ItemWidgetPool.Num(); ++i)
	{
		if (ItemWidgetPool[i])
		{
			ItemWidgetPool[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
