// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ItemAddInfoListWidget.h"

#include "ItemAddInfoWidget.h"
#include "Component/InventoryComponent.h"
#include "Components/VerticalBox.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UItemAddInfoListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		return;
	}

	ItemAddHandle = MessageSubsystem->RegisterListener<FItemAddMessage>(
		TAG_Message_Channel_ItemAddInfo,
		this,
		&UItemAddInfoListWidget::OnAddItemMessageReceived
	);

	TryInventoryComponent();
}

void UItemAddInfoListWidget::NativeDestruct()
{
	
	if (ItemAddHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(ItemAddHandle);
		}
	}
	Super::NativeDestruct();
}

void UItemAddInfoListWidget::TryInventoryComponent()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS)
	{
		// 다음 틱에 다시 시도
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryInventoryComponent(); });
		return;
	}

	CachedInventory = PS->FindComponentByClass<UInventoryComponent>();
	
	if (!CachedInventory)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryInventoryComponent(); });
		return;
	}

	InitializePopupPool();
}

void UItemAddInfoListWidget::InitializePopupPool()
{
	if (!ItemPopupWidget)
	{
		return;
	}

	if (!PopupContainer)
	{
		return;
	}

	for (int32 i = 0; i < 5; ++i)
	{
		UItemAddInfoWidget* Widget = CreateWidget<UItemAddInfoWidget>(GetOwningPlayer(), ItemPopupWidget);
		if (Widget)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			PopupContainer->AddChild(Widget);
			ItemPopupPool.Add(Widget);
		}
	}
}

void UItemAddInfoListWidget::ProcessPendingMessages()
{
	if (PendingMessages.IsEmpty())
            return;
    
        // 빈 팝업 찾기
        UItemAddInfoWidget* PopupToUse = GetWidgetPool();
        if (!PopupToUse)
        {
            // 풀 없으면 1.5초 후 재시도
            if (!GetWorld()->GetTimerManager().IsTimerActive(RetryTimerHandle))
            {
                GetWorld()->GetTimerManager().SetTimer(RetryTimerHandle, this, &UItemAddInfoListWidget::ProcessPendingMessages, 1.5f, false);
            }
            return;
        }
    
        // 메시지 처리
        FItemAddMessage Message;
        if (PendingMessages.Dequeue(Message))
        {
            USKInventoryItemData* ItemData = CachedInventory->GetItemDataByID(Message.ItemID);
            if (ItemData)
            {
                PopupToUse->SettingViewItem(ItemData->ItemName, Message.ItemCount, ItemData->ItemIcon);
            }
        }
    
        // 메시지 남아있으면 즉시 재호출
        if (!PendingMessages.IsEmpty())
        {
            ProcessPendingMessages();
        }
}

UItemAddInfoWidget* UItemAddInfoListWidget::GetWidgetPool()
{
	UItemAddInfoWidget* PopupToUse = nullptr;

	for (int32 i = 0; i < ItemPopupPool.Num(); ++i)
	{
		UItemAddInfoWidget* Popup = ItemPopupPool[i];
		ESlateVisibility Vis = Popup->GetVisibility();

		if (Vis == ESlateVisibility::Collapsed)
		{
			PopupToUse = Popup;
			break;
		}
	}

	return PopupToUse;
}

void UItemAddInfoListWidget::OnAddItemMessageReceived(FGameplayTag Channel, const FItemAddMessage& Message)
{
	PendingMessages.Enqueue(Message);
	ProcessPendingMessages();
}
