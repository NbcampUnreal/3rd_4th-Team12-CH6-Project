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
	Super::NativeDestruct();
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
 
	if (QuickSlot1->GetItemID() != CurrentQuickSlots[0].ItemID || CurrentQuickSlots[0].ItemID == -1)
	{
		QuickSlot1->SettingItem(CurrentQuickSlots[0].ItemID, CurrentQuickSlots[0].Count);
	}
 
	if (QuickSlot2->GetItemID() != CurrentQuickSlots[1].ItemID|| CurrentQuickSlots[1].ItemID == -1)
	{
		QuickSlot2->SettingItem(CurrentQuickSlots[1].ItemID, CurrentQuickSlots[1].Count);
	}
 
	if (QuickSlot3->GetItemID() != CurrentQuickSlots[2].ItemID || CurrentQuickSlots[2].ItemID == -1)
	{
		QuickSlot3->SettingItem(CurrentQuickSlots[2].ItemID, CurrentQuickSlots[2].Count);
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
