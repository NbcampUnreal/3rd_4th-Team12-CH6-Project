// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipMainListSlotWidget.h"

#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"
#include "EquipmentItemEquipWidget.h"
#include "EquipmentItemQuickWidget.h"

#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipMainListSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
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
		&UEquipMainListSlotWidget::OnSwitchLayoutMessageReceived
	);

	InteractionHandle = MessageSubsystem->RegisterListener<FUIInteractionMoveMessage>(
		TAG_Message_Channel_UIInteraction,
		this,
		&UEquipMainListSlotWidget::OnInteractionMessageReceived
	);

	SlotList.Empty();
	SlotPositions.Empty();
	
	// Row 0
	SlotList.Add(WeaponSlot);
	SlotPositions.Add(FIntPoint(0, 0));
	WeaponSlot->ParentWidget = this;

	SlotList.Add(Accessory1Slot);
	SlotPositions.Add(FIntPoint(0, 1));
	Accessory1Slot->ParentWidget = this;

	SlotList.Add(Accessory2Slot);
	SlotPositions.Add(FIntPoint(0, 2));
	Accessory2Slot->ParentWidget = this;

	// Row 1
	SlotList.Add(HelmetSlot);
	SlotPositions.Add(FIntPoint(1, 0));
	HelmetSlot->ParentWidget = this;

	SlotList.Add(ChestSlot);
	SlotPositions.Add(FIntPoint(1, 1));
	ChestSlot->ParentWidget = this;

	SlotList.Add(LegSlot);
	SlotPositions.Add(FIntPoint(1, 2));
	LegSlot->ParentWidget = this;

	SlotList.Add(BootsSlot);
	SlotPositions.Add(FIntPoint(1, 3));
	BootsSlot->ParentWidget = this;

	// Row 2
	SlotList.Add(QuickSlot1);
	SlotPositions.Add(FIntPoint(2, 0));
	QuickSlot1->ParentWidget = this;

	SlotList.Add(QuickSlot2);
	SlotPositions.Add(FIntPoint(2, 1));
	QuickSlot2->ParentWidget = this;

	SlotList.Add(QuickSlot3);
	SlotPositions.Add(FIntPoint(2, 2));
	QuickSlot3->ParentWidget = this;

	// 각 Slot들에게 인덱스 전달
	for (int32 i = 0; i < SlotList.Num(); i++)
	{
		SlotList[i]->SetIndex(i);
	}

	CurrentIndex = 0;
	SetIndexHover(CurrentIndex);
	
	TryCachedComponent();
}

void UEquipMainListSlotWidget::NativeDestruct()
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

void UEquipMainListSlotWidget::NotifyIndex(int32 Index)
{
	if (CurrentIndex != Index)
	{
		SlotList[CurrentIndex]->HoverImageVisible(false);
	}
	CurrentIndex = Index;
}

void UEquipMainListSlotWidget::TryCachedComponent()
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
	SettingSlots();
}

void UEquipMainListSlotWidget::SettingSlots()
{
	if (WeaponSlot)
	{
		WeaponSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Weapon, CachedInventory);
	}
	if (HelmetSlot)
	{
		HelmetSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Helmet, CachedInventory);
	}
	if (ChestSlot)
	{
		ChestSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Chest, CachedInventory);
	}
	if (LegSlot)
	{
		LegSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Leg, CachedInventory);
	}
	if (BootsSlot)
	{
		BootsSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Boots, CachedInventory);
	}
	if (Accessory1Slot)
	{
		Accessory1Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory1,CachedInventory);
	}
	if (Accessory2Slot)
	{
		Accessory2Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory2,CachedInventory);
	}

	if (QuickSlot1)
	{
		QuickSlot1->SettingSlot(EInventoryItemType::Consumable, 0, CachedInventory);
	}
	if (QuickSlot2)
	{
		QuickSlot2->SettingSlot(EInventoryItemType::Consumable, 1, CachedInventory);
	}
	if (QuickSlot3)
	{
		QuickSlot3->SettingSlot(EInventoryItemType::Consumable, 2, CachedInventory);
	}
	
	RefreshEquipMainSlots();
}

void UEquipMainListSlotWidget::RefreshEquipMainSlots()
{
	UE_LOG(LogTemp, Log, TEXT("RefreshEquipMainSlots"));
	if (!CachedEquipment || !CachedQuickSlot)
	{
		return;
	}

	CheckEquipSlot(WeaponSlot);
	CheckEquipSlot(HelmetSlot);
	CheckEquipSlot(ChestSlot);
	CheckEquipSlot(LegSlot);
	CheckEquipSlot(BootsSlot);
	CheckEquipSlot(Accessory1Slot);
	CheckEquipSlot(Accessory2Slot);
	
	CheckQuickSlot();
}

void UEquipMainListSlotWidget::CheckEquipSlot(UEquipmentItemEquipWidget* CheckWidget)
{
	if (!CheckWidget || !CachedEquipment)
		return;
		
	const FEquipmentSlotData* CurrentSlotData = CachedEquipment->GetEquipment(CheckWidget->GetSlotType());

	if (!CurrentSlotData)
		return;
		
	// Compare with currently displayed instance
	if (CheckWidget->GetCurrentInstance() != CurrentSlotData->EquipmentInstance || CurrentSlotData->ItemID == -1)
	{ 
		CheckWidget->SettingItem(CurrentSlotData->ItemID , CurrentSlotData->EquipmentInstance);
	}
}

void UEquipMainListSlotWidget::CheckQuickSlot()
{
	if (!CachedQuickSlot || !CachedEquipment)
		return;

	TArray<FQuickSlot> CurrentQuickSlots = CachedQuickSlot->GetQuickSlots();
 
	if (QuickSlot1->GetItemID() != CurrentQuickSlots[0].ItemID ||
		QuickSlot1->GetItemQuantity() != CurrentQuickSlots[0].Count)
	{
		QuickSlot1->SettingItem(CurrentQuickSlots[0].ItemID, CurrentQuickSlots[0].Count);
	}
 
	if (QuickSlot2->GetItemID() != CurrentQuickSlots[1].ItemID ||
		QuickSlot2->GetItemQuantity() != CurrentQuickSlots[1].Count)
	{
		QuickSlot2->SettingItem(CurrentQuickSlots[1].ItemID, CurrentQuickSlots[1].Count);
	}
 
	if (QuickSlot3->GetItemID() != CurrentQuickSlots[2].ItemID ||
		QuickSlot3->GetItemQuantity() != CurrentQuickSlots[2].Count)
	{
		QuickSlot3->SettingItem(CurrentQuickSlots[2].ItemID, CurrentQuickSlots[2].Count);
	}
	
}

void UEquipMainListSlotWidget::MoveIndex(int32 Index)
{
	if (SlotList.Num() == 0) return;

	int32 PrevIndex = CurrentIndex;

	int32 CurrentRow = SlotPositions[CurrentIndex].X;
	int32 CurrentCol = SlotPositions[CurrentIndex].Y;

	int32 BestIndex = CurrentIndex;
	int32 BestDistance = 9999;
	
	int32 TargetRow = CurrentRow;
	int32 TargetCol = CurrentCol;

	switch (Index)
	{
		case 0: TargetRow = CurrentRow - 1; break; // Up
		case 1: TargetRow = CurrentRow + 1; break; // Down
		case 2: TargetCol = CurrentCol - 1; break; // Left
		case 3: TargetCol = CurrentCol + 1; break; // Right
		default: break;
	}

	// 가장 가까운 슬롯 찾기
	for (int32 i = 0; i < SlotList.Num(); i++)
	{
		FIntPoint Pos = SlotPositions[i];

		// 이동 방향에 맞는 슬롯만 허용
		if (Index == 0 && Pos.X >= CurrentRow) continue; // Up
		if (Index == 1 && Pos.X <= CurrentRow) continue; // Down
		if (Index == 2 && Pos.Y >= CurrentCol) continue; // Left
		if (Index == 3 && Pos.Y <= CurrentCol) continue; // Right

		int32 Dist = FMath::Abs(Pos.X - TargetRow) * 10 + FMath::Abs(Pos.Y - TargetCol);

		if (Dist < BestDistance)
		{
			BestDistance = Dist;
			BestIndex = i;
		}
	}

	// 후보가 없으면 그대로
	CurrentIndex = BestIndex;
	
	SetIndexUnHover(PrevIndex);
	SetIndexHover(CurrentIndex);
}

void UEquipMainListSlotWidget::SetIndexHover(int32 Index)
{
	FGeometry DummyGeometry;
	FPointerEvent DummyPointerEvent;
	SlotList[Index]->NativeOnMouseEnter(DummyGeometry, DummyPointerEvent);
}

void UEquipMainListSlotWidget::SetIndexUnHover(int32 Index)
{
	FPointerEvent DummyPointerEvent;
	SlotList[Index]->NativeOnMouseLeave(DummyPointerEvent);
}

void UEquipMainListSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_EquipmentMain)
	{
		return;
	}
	RefreshEquipMainSlots();
}

void UEquipMainListSlotWidget::OnInteractionMessageReceived(FGameplayTag Channel,
	const FUIInteractionMoveMessage& Message)
{
	if (Message.Type != EUIMessageType::Equipmentmain)
	{
		return;
	}
		
	if (Message.MoveDirection == 4)
	{
		SlotList[CurrentIndex]->OnClicked();
	}
	else
	{
		MoveIndex(Message.MoveDirection);;
	}
}
