// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "EquipMainListSlotWidget.generated.h"

class UEquipmentItemQuickWidget;
class UInventoryComponent;
class UEquipmentItemEquipWidget;
class UEquipMainItemWidget;
class UEquipmentComponent;
class UQuickSlotComponent;
/**
 * 
 */
UCLASS()
class SK_API UEquipMainListSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	void TryCachedComponent();

	void SettingSlots();
	
	void RefreshEquipMainSlots();

	void CheckEquipSlot(UEquipmentItemEquipWidget* CheckWidget);

	void CheckQuickSlot();

	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);
	
	UPROPERTY()
	UEquipmentComponent* CachedEquipment;

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY()
	UInventoryComponent* CachedInventory;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* WeaponSlot;
		
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* HelmetSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* ChestSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* LegSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* BootsSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* Accessory1Slot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipmentItemEquipWidget* Accessory2Slot;
 
	// 퀵슬롯 위젯들
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipmentItemQuickWidget* QuickSlot1;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipmentItemQuickWidget* QuickSlot2;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipmentItemQuickWidget* QuickSlot3;
};
