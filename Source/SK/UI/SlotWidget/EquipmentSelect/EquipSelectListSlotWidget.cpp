// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipSelectListSlotWidget.h"

#include "EquipSelectItemWidget.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipSelectListSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryCachedComponent();

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

void UEquipSelectListSlotWidget::TryCachedComponent()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS)
	{
		// 다음 틱에 다시 시도
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedEquipment = PS->FindComponentByClass<UEquipmentComponent>();
	
	if (!CachedEquipment)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedQuickSlot = PS->FindComponentByClass<UQuickSlotComponent>();
	
	if (!CachedQuickSlot)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedInventory = PS->FindComponentByClass<UInventoryComponent>();
	
	if (!CachedInventory)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}
}

void UEquipSelectListSlotWidget::OnItemSwitchMessageReceived(FGameplayTag Channel, const FItemSwitchMessage& Message)
{
	CurrentEquipmentSlotType = Message.EquipmentType;
	CurrentItemType = Message.ItemType;
	CurrentQuickSlotNumber = Message.QuickSlotNumber;
	RefreshInventory();
	if (TypeText)
	{
		FText DisplayText;
		if (Message.ItemType == EInventoryItemType::Consumable )
		{
			//퀵 슬롯 x 번 / 장착
			DisplayText = FText::FromString(FString::Printf(TEXT("퀵 슬롯 %d번 장착"), Message.QuickSlotNumber));
		}
		else
		{
			// 장비: 무기 슬롯 장착
			switch (Message.EquipmentType)
			{
			case EEquipmentSlotType::Weapon:
				DisplayText = FText::FromString(TEXT("무기 슬롯 장착"));
				break;

			case EEquipmentSlotType::Helmet:
				DisplayText = FText::FromString(TEXT("투구 슬롯 장착"));
				break;

			case EEquipmentSlotType::Chest:
				DisplayText = FText::FromString(TEXT("갑옷 슬롯 장착"));
				break;
			case EEquipmentSlotType::Leg:
				DisplayText = FText::FromString(TEXT("하의 슬롯 장착"));
				break;
			case EEquipmentSlotType::Boots:
				DisplayText = FText::FromString(TEXT("신발 슬롯 장착"));
				break;
			case EEquipmentSlotType::Accessory1:
				DisplayText = FText::FromString(TEXT("장신구1 슬롯 장착"));
				break;
			case EEquipmentSlotType::Accessory2:
				DisplayText = FText::FromString(TEXT("장신구2 슬롯 장착"));
				break;
				
			default:
				DisplayText = FText::FromString(TEXT("기타 장비 장착"));
				break;
			}
		}

		TypeText->SetText(DisplayText);
		
	}
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
        UEquipSelectItemWidget* NewWidget = CreateWidget<UEquipSelectItemWidget>(this, ItemWidgetClass);
        NewWidget->SetVisibility(ESlateVisibility::Visible);
    	NewWidget->SendComponent(CachedQuickSlot, CachedEquipment, CachedInventory);
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
 
        UEquipSelectItemWidget* ItemWidget = ItemWidgetPool[i];
 
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
