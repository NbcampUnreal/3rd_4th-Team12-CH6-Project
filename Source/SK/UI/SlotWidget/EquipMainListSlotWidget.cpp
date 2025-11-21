// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/EquipMainListSlotWidget.h"

#include "EquipMainItemWidget.h"
#include "Component/EquipmentComponent.h"
#include "Component/QuickSlotComponent.h"

#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipMainListSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WeaponSlot)
	{
		WeaponSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Weapon);
	}
	if (HelmetSlot)
	{
		HelmetSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Helmet);
	}
	if (ChestSlot)
	{
		ChestSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Chest);
	}
	if (LegSlot)
	{
		LegSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Leg);
	}
	if (BootsSlot)
	{
		BootsSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Boots);
	}
	if (Accessory1Slot)
	{
		Accessory1Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory1);
	}
	if (Accessory2Slot)
	{
		Accessory2Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory2);
	}
	if (QuickSlot1)
	{
		QuickSlot1->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None);
	}
	if (QuickSlot2)
	{
		QuickSlot2->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None);
	}
	if (QuickSlot3)
	{
		QuickSlot3->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None);
	}

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
	
	TryCachedComponent();
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

void UEquipMainListSlotWidget::CheckEquipSlot(UEquipMainItemWidget* CheckWidget)
{
	if (!CheckWidget || !CachedEquipment)
		return;
		
	const FEquipmentSlotData* CurrentSlotData = CachedEquipment->GetEquipment(CheckWidget->CurrentEquipSlotType);

	if (!CurrentSlotData)
		return;
		
	// Compare with currently displayed instance
	if (CheckWidget->CurrentEquipMainItem.CurrentEquipInstance != CurrentSlotData->EquipmentInstance)
	{ 
		CheckWidget->SetItem(CurrentSlotData->ItemID ,CachedEquipment);
	}
}

void UEquipMainListSlotWidget::CheckQuickSlot()
{
	if (!CachedQuickSlot || !CachedEquipment)
		return;

	TArray<FQuickSlot> CurrentQuickSlots = CachedQuickSlot->GetQuickSlots();

	// QuickSlot1 로그
	UE_LOG(LogTemp, Warning, TEXT("QuickSlot1: CurrentItemID=%d, TargetItemID=%d"),
		QuickSlot1->CurrentEquipMainItem.CurrentItemID,
		CurrentQuickSlots.IsValidIndex(0) ? CurrentQuickSlots[0].ItemID : -1);
 
	if (CurrentQuickSlots.IsValidIndex(0) && QuickSlot1->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[0].ItemID)
	{
		QuickSlot1->SetItem(CurrentQuickSlots[0].ItemID, CachedEquipment);
		UE_LOG(LogTemp, Warning, TEXT("QuickSlot1 SetItem: NewItemID=%d"), CurrentQuickSlots[0].ItemID);
	}
 
	// QuickSlot2 로그
	UE_LOG(LogTemp, Warning, TEXT("QuickSlot2: CurrentItemID=%d, TargetItemID=%d"),
		QuickSlot2->CurrentEquipMainItem.CurrentItemID,
		CurrentQuickSlots.IsValidIndex(1) ? CurrentQuickSlots[1].ItemID : -1);
 
	if (CurrentQuickSlots.IsValidIndex(1) && QuickSlot2->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[1].ItemID)
	{
		QuickSlot2->SetItem(CurrentQuickSlots[1].ItemID, CachedEquipment);
		UE_LOG(LogTemp, Warning, TEXT("QuickSlot2 SetItem: NewItemID=%d"), CurrentQuickSlots[1].ItemID);
	}
 
	// QuickSlot3 로그
	UE_LOG(LogTemp, Warning, TEXT("QuickSlot3: CurrentItemID=%d, TargetItemID=%d"),
		QuickSlot3->CurrentEquipMainItem.CurrentItemID,
		CurrentQuickSlots.IsValidIndex(2) ? CurrentQuickSlots[2].ItemID : -1);
 
	if (CurrentQuickSlots.IsValidIndex(2) && QuickSlot3->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[2].ItemID)
	{
		QuickSlot3->SetItem(CurrentQuickSlots[2].ItemID, CachedEquipment);
		UE_LOG(LogTemp, Warning, TEXT("QuickSlot3 SetItem: NewItemID=%d"), CurrentQuickSlots[2].ItemID);
	}
	
}

void UEquipMainListSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_EquipmentMain)
	{
		return;
	}
	RefreshEquipMainSlots();
}
