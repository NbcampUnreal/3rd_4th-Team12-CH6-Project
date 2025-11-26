// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/InventoryListSlotWidget.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Component/InventoryComponent.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UInventoryListSlotWidget::NativeConstruct()
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
	LayoutSwitchHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
		TAG_Message_Channel_SwitchLayout,
		this,
		&UInventoryListSlotWidget::OnSwitchLayoutMessageReceived
	);
}

void UInventoryListSlotWidget::TryCachedInventory()
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

	if (ButtonAll)
	{
		ButtonAll->OnClicked.AddDynamic(this, &UInventoryListSlotWidget::OnButtonAllClicked);
	}
	if (ButtonEquipment)
	{
		ButtonEquipment->OnClicked.AddDynamic(this, &UInventoryListSlotWidget::OnButtonEquipmentClicked);
	}
	if (ButtonConsumables)
	{
		ButtonConsumables->OnClicked.AddDynamic(this, &UInventoryListSlotWidget::OnButtonConsumablesClicked);
	}
	if (ButtonOthers)
	{
		ButtonOthers->OnClicked.AddDynamic(this, &UInventoryListSlotWidget::OnButtonOthersClicked);
	}
	
	RefreshInventory();
}

void UInventoryListSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_Inventory)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryListSlotWidget No Layout Inventory"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventoryListSlotWidget Yes Layout Inventory"));
	RefreshInventory();
	
}

void UInventoryListSlotWidget::RefreshInventory()
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
	
	TArray<FInventorySlot> CachedInventoryArray;
	switch (CurrentFilter)
	{
		case EInventoryFilterType::All:
			CachedInventoryArray = CachedInventory->GetAllItems();
			break;
		case EInventoryFilterType::Consumables:
			CachedInventoryArray = CachedInventory->GetItemsByType(EInventoryItemType::Consumable);
			break;
		case EInventoryFilterType::Equipment:
			CachedInventoryArray = CachedInventory->GetItemsByType(EInventoryItemType::Equipment);
			break;
		case EInventoryFilterType::Others:
			CachedInventoryArray = CachedInventory->GetItemsByType(EInventoryItemType::Misc);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown inventory filter"));
			CachedInventoryArray.Empty();
			break;
	}


    int32 ItemCount = CachedInventoryArray.Num();
    int32 TotalSlots = FMath::Max(MinSlotCount, FMath::CeilToInt(float(ItemCount) / ItemsPerRow) * ItemsPerRow);
 
    // 아이템 위젯 풀 확보
    while (ItemWidgetPool.Num() < TotalSlots)
    {
        UInventoryItemWidget* NewWidget = CreateWidget<UInventoryItemWidget>(this, ItemWidgetClass);
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
                RowSlot->SetPadding(FMargin(10.f, 0.f, 10.f, 0.f));
                RowSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
                RowSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
                RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            }
        }
 
        UInventoryItemWidget* ItemWidget = ItemWidgetPool[i];
 
        if (i < ItemCount)
        {
            USKInventoryItemData* CurrentItem = CachedInventory->GetItemDataByID(CachedInventoryArray[i].ItemID);
 
            FInventoryItemForWidget ItemData;
            ItemData.ItemName = CurrentItem->ItemID;
            ItemData.Quantity = CachedInventoryArray[i].Count;
            ItemData.Icon = CurrentItem->ItemIcon;
			ItemData.ItemID = CachedInventoryArray[i].ItemID;
            ItemWidget->SetItem(ItemData);
            ItemWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            FInventoryItemForWidget EmptyItem;
            EmptyItem.ItemName = "NO_Item";
            EmptyItem.Quantity = 0;
            EmptyItem.Icon = nullptr;
        	EmptyItem.ItemID = -1;
            ItemWidget->SetItem(EmptyItem);
            ItemWidget->SetVisibility(ESlateVisibility::Visible);
        }
 
        UHorizontalBoxSlot* ItemSlot = CurrentRow->AddChildToHorizontalBox(ItemWidget);
        if (ItemSlot)
        {
            ItemSlot->SetPadding(FMargin(10.f, 0.f, 10.f, 0.f));
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

void UInventoryListSlotWidget::OnButtonAllClicked()
{
	if (CurrentFilter == EInventoryFilterType::All)
		return;
	CurrentFilter = EInventoryFilterType::All;
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonEquipmentClicked()
{
	if (CurrentFilter == EInventoryFilterType::Equipment)
		return;
	CurrentFilter = EInventoryFilterType::Equipment;
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonConsumablesClicked()
{
	if (CurrentFilter == EInventoryFilterType::Consumables)
		return;
	CurrentFilter = EInventoryFilterType::Consumables;
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonOthersClicked()
{
	if (CurrentFilter == EInventoryFilterType::Others)
		return;
	CurrentFilter = EInventoryFilterType::Others;
	RefreshInventory();
}
