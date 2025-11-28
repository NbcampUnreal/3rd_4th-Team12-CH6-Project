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

	UE_LOG(LogTemp, Warning, TEXT("ItemAddInfoListWidget::NativeConstruct"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr"));
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is nullptr"));
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("MessageSubsystem is nullptr"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("RegisterListener 호출됨"));

	ItemAddHandle = MessageSubsystem->RegisterListener<FItemAddMessage>(
		TAG_Message_Channel_ItemAddInfo,
		this,
		&UItemAddInfoListWidget::OnAddItemMessageReceived
	);

	TryInventoryComponent();
}

void UItemAddInfoListWidget::NativeDestruct()
{
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
	UE_LOG(LogTemp, Warning, TEXT("InitializePopupPool 호출됨"));

	if (!ItemPopupWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPopupWidget SoftPtr 비어있음"));
		return;
	}

	if (!PopupContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("PopupContainer가 UMG에서 바인딩 안됨"));
		return;
	}

	for (int32 i = 0; i < 5; ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget %d 생성 시도"), i);

		UItemAddInfoWidget* Widget = CreateWidget<UItemAddInfoWidget>(GetOwningPlayer(), ItemPopupWidget);
		if (Widget)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			PopupContainer->AddChild(Widget);
			ItemPopupPool.Add(Widget);

			UE_LOG(LogTemp, Warning, TEXT("Widget %d 생성 완료"), i);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Widget %d 생성 실패"), i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Popup Pool Size = %d"), ItemPopupPool.Num());
}

void UItemAddInfoListWidget::OnAddItemMessageReceived(FGameplayTag Channel, const FItemAddMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("=== 아이템 추가 메시지 수신 ==="));
	UE_LOG(LogTemp, Warning, TEXT("ItemID: %d, Count: %d"), Message.ItemID, Message.ItemCount);

	if (!CachedInventory)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedInventory 없음! TryInventoryComponent가 실행 안됨"));
		return;
	}

	// 빈 팝업 찾기
	UItemAddInfoWidget* PopupToUse = nullptr;

	for (int32 i = 0; i < ItemPopupPool.Num(); ++i)
	{
		UItemAddInfoWidget* Popup = ItemPopupPool[i];
		ESlateVisibility Vis = Popup->GetVisibility();

		UE_LOG(LogTemp, Warning, TEXT("Popup[%d] Visibility = %d"), i, (int32)Vis);

		if (Vis == ESlateVisibility::Collapsed)
		{
			PopupToUse = Popup;
			UE_LOG(LogTemp, Warning, TEXT("Popup[%d] 선택됨"), i);
			break;
		}
	}

	if (!PopupToUse)
	{
		UE_LOG(LogTemp, Error, TEXT("사용 가능한 Popup 없음 (모두 표시 중)"));
		return;
	}

	// 아이템 데이터 가져오기
	USKInventoryItemData* ItemData = CachedInventory->GetItemDataByID(Message.ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemData NULL (ItemID %d)"), Message.ItemID);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SettingViewItem 호출 (Name: %s, Count: %d)"),
		*ItemData->ItemName.ToString(), Message.ItemCount);

	PopupToUse->SettingViewItem(ItemData->ItemName, Message.ItemCount, ItemData->ItemIcon);
}
