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
		WeaponSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Weapon, -1);
	}
	if (HelmetSlot)
	{
		HelmetSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Helmet, -1);
	}
	if (ChestSlot)
	{
		ChestSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Chest, -1);
	}
	if (LegSlot)
	{
		LegSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Leg, -1);
	}
	if (BootsSlot)
	{
		BootsSlot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Boots, -1);
	}
	if (Accessory1Slot)
	{
		Accessory1Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory1,-1);
	}
	if (Accessory2Slot)
	{
		Accessory2Slot->SettingSlot(EInventoryItemType::Equipment, EEquipmentSlotType::Accessory2,-1);
	}
	if (QuickSlot1)
	{
		QuickSlot1->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None,0);
	}
	if (QuickSlot2)
	{
		QuickSlot2->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None,1);
	}
	if (QuickSlot3)
	{
		QuickSlot3->SettingSlot(EInventoryItemType::Consumable, EEquipmentSlotType::None,2);
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
	if (CheckWidget->CurrentEquipMainItem.CurrentEquipInstance != CurrentSlotData->EquipmentInstance || CurrentSlotData->ItemID == -1)
	{ 
		CheckWidget->SetItem(CurrentSlotData->ItemID ,CachedEquipment);
	}
}

void UEquipMainListSlotWidget::CheckQuickSlot()
{
	if (!CachedQuickSlot || !CachedEquipment)
		return;

	TArray<FQuickSlot> CurrentQuickSlots = CachedQuickSlot->GetQuickSlots();
 
	if (QuickSlot1->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[0].ItemID || CurrentQuickSlots[0].ItemID == -1)
	{
		QuickSlot1->SetItem(CurrentQuickSlots[0].ItemID, CachedEquipment);
	}
 
	if (QuickSlot2->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[1].ItemID|| CurrentQuickSlots[1].ItemID == -1)
	{
		QuickSlot2->SetItem(CurrentQuickSlots[1].ItemID, CachedEquipment);
	}
 
	if (QuickSlot3->CurrentEquipMainItem.CurrentItemID != CurrentQuickSlots[2].ItemID || CurrentQuickSlots[2].ItemID == -1)
	{
		QuickSlot3->SetItem(CurrentQuickSlots[2].ItemID, CachedEquipment);
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
