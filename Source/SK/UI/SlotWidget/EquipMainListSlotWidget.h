// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "EquipMainListSlotWidget.generated.h"

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
	
protected:
	void TryCachedComponent();

	void RefreshEquipMainSlots();

	void CheckEquipSlot(UEquipMainItemWidget* CheckWidget);

	void CheckQuickSlot();

	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);
	
	UPROPERTY()
	UEquipmentComponent* CachedEquipment;

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* WeaponSlot;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* HelmetSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* ChestSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* LegSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* BootsSlot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* Accessory1Slot;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Equipment|Slots")
	UEquipMainItemWidget* Accessory2Slot;
 
	// 퀵슬롯 위젯들
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipMainItemWidget* QuickSlot1;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipMainItemWidget* QuickSlot2;
 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "QuickSlots")
	UEquipMainItemWidget* QuickSlot3;
};
