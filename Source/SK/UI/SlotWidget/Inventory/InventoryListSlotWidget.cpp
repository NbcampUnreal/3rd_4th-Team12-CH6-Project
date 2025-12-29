// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryListSlotWidget.h"

#include "InventoryItemWidget.h"
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

	InteractionHandle = MessageSubsystem->RegisterListener<FUIInteractionMoveMessage>(
		TAG_Message_Channel_UIInteraction,
		this,
		&UInventoryListSlotWidget::OnInteractionMessageReceived
	);
}

void UInventoryListSlotWidget::NativeDestruct()
{
	if (LayoutSwitchHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(LayoutSwitchHandle);
		}
	}

	if (InteractionHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(InteractionHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UInventoryListSlotWidget::NotifyIndex(int32 Index)
{
	if (CurrentIndex != Index)
	{
		ItemWidgetPool[CurrentIndex]->HoverImageVisible(false);
	}
	CurrentIndex = Index;
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

void UInventoryListSlotWidget::OnInteractionMessageReceived(FGameplayTag Channel,
	const FUIInteractionMoveMessage& Message)
{
	if (Message.Type != EUIMessageType::Inventory)
	{
		return;
	}
		
	if (Message.MoveDirection == 4)
	{
		//ToDO: 추후 인벤토리에서 바로 퀵슬롯, 장비 장착이 되게끔 진행
	}
	else
	{
		MoveIndex(Message.MoveDirection);;
	}
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
    	NewWidget->ParentWidget = this;
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

    	ItemWidget->WidgetIndex = i;
    	ItemWidget->HoverImageVisible(false);
    	
        if (i < ItemCount)
        {
            USKInventoryItemData* CurrentItem = CachedInventory->GetItemDataByID(CachedInventoryArray[i].ItemID);
        	if (CurrentItem)
        	{
        		FInventoryItemForWidget ItemData;
        		ItemData.ItemName = CurrentItem->ItemID;
        		ItemData.Quantity = CachedInventoryArray[i].Count;
        		ItemData.Icon = CurrentItem->ItemIcon;
        		ItemData.ItemID = CachedInventoryArray[i].ItemID;
        		ItemWidget->SetItem(ItemData);
        	}
        	else
        	{
        		UE_LOG(LogTemp, Warning, TEXT("GetItemDataByID returned nullptr for ItemID %d"), CachedInventoryArray[i].ItemID);
        		FInventoryItemForWidget EmptyItem;
        		EmptyItem.ItemName = "Invalid_Item";
        		EmptyItem.Quantity = 0;
        		EmptyItem.Icon = nullptr;
        		EmptyItem.ItemID = -1;
        		ItemWidget->SetItem(EmptyItem);
        	}
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

	VisibleWidgetCount = TotalSlots;
	CurrentIndex = 0;
	SetIndexHover(CurrentIndex);
	
	for (int32 i = TotalSlots; i < ItemWidgetPool.Num(); ++i)
	{
		if (ItemWidgetPool[i])
		{
			ItemWidgetPool[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryListSlotWidget::SetIndexHover(int32 Index)
{
	FGeometry DummyGeometry;
	FPointerEvent DummyPointerEvent;
	ItemWidgetPool[Index]->NativeOnMouseEnter(DummyGeometry, DummyPointerEvent);
}

void UInventoryListSlotWidget::SetIndexUnHover(int32 Index)
{
	FPointerEvent DummyPointerEvent;
	ItemWidgetPool[Index]->NativeOnMouseLeave(DummyPointerEvent);
}

void UInventoryListSlotWidget::MoveIndex(int32 Index)
{
	if (VisibleWidgetCount == 0 || ItemWidgetPool.Num() == 0)
	{
		return;
	}

	int32 PreviousIndex = CurrentIndex;
	
	switch (Index)
	{
	case 0: // Up
		{
			CurrentIndex -= ItemsPerRow;
			if (CurrentIndex < 0)
			{
				// 마지막 줄로 이동 (위로 이동해서 벗어난 경우)
				int32 RowCount = FMath::CeilToInt((float)VisibleWidgetCount / ItemsPerRow);
				CurrentIndex += RowCount * ItemsPerRow;

				// 이동한 인덱스가 실제 아이템 개수보다 클 수 있으니 조정
				if (CurrentIndex >= VisibleWidgetCount)
				{
					CurrentIndex = VisibleWidgetCount - 1;
				}
			}
			break;
		}

	case 1: // Down
		{
			CurrentIndex += ItemsPerRow;

			// 만약 인덱스를 넘어가면 첫 번째 줄의 동일한 칼럼으로 이동
			if (CurrentIndex >= VisibleWidgetCount)
			{
				CurrentIndex = CurrentIndex % ItemsPerRow;

				// 만약 그 칼럼에 아이템이 없으면 마지막 아이템으로 이동
				if (CurrentIndex >= VisibleWidgetCount)
				{
					CurrentIndex = VisibleWidgetCount - 1;
				}
			}
			break;
		}

	case 2: // Left
		{
			CurrentIndex--;

			if (CurrentIndex < 0)
			{
				CurrentIndex = VisibleWidgetCount - 1;
			}
			break;
		}

	case 3: // Right
		{
			CurrentIndex++;

			if (CurrentIndex >= VisibleWidgetCount)
			{
				CurrentIndex = 0;
			}
			break;
		}
	default:
		break;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("MoveIndex: Prev=%d  Current=%d  Visible=%d"), PreviousIndex, CurrentIndex, VisibleWidgetCount);
	SetIndexUnHover(PreviousIndex);
	SetIndexHover(CurrentIndex);

	ScrollToIndex(CurrentIndex);
}

void UInventoryListSlotWidget::ScrollToIndex(int32 Index)
{
	if (!InventoryScroll || CurrentIndex >= VisibleWidgetCount) return;

	UWidget* TargetWidget = ItemWidgetPool[Index];
	if (!TargetWidget) return;

	InventoryScroll->ScrollWidgetIntoView(TargetWidget, true, EDescendantScrollDestination::IntoView);
}

void UInventoryListSlotWidget::OnButtonAllClicked()
{
	if (CurrentFilter == EInventoryFilterType::All)
		return;
	CurrentFilter = EInventoryFilterType::All;
	SetIndexUnHover(CurrentIndex);
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonEquipmentClicked()
{
	if (CurrentFilter == EInventoryFilterType::Equipment)
		return;
	CurrentFilter = EInventoryFilterType::Equipment;
	SetIndexUnHover(CurrentIndex);
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonConsumablesClicked()
{
	if (CurrentFilter == EInventoryFilterType::Consumables)
		return;
	CurrentFilter = EInventoryFilterType::Consumables;
	SetIndexUnHover(CurrentIndex);
	RefreshInventory();
}

void UInventoryListSlotWidget::OnButtonOthersClicked()
{
	if (CurrentFilter == EInventoryFilterType::Others)
		return;
	CurrentFilter = EInventoryFilterType::Others;
	SetIndexUnHover(CurrentIndex);
	RefreshInventory();
}
