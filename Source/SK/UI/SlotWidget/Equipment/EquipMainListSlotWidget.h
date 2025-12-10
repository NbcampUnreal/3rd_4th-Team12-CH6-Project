// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "EquipMainListSlotWidget.generated.h"

class UEquipmentItemBaseWidget;
class UEquipmentItemQuickWidget;
class UInventoryComponent;
class UEquipmentItemEquipWidget;
class UEquipMainItemWidget;
class UEquipmentComponent;
class UQuickSlotComponent;

struct FEquipSlotPosition
{
	int32 Row;
	int32 Col;
};

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
	
	void NotifyIndex(int32 Index);
	
protected:
	void TryCachedComponent();

	void SettingSlots();
	
	void RefreshEquipMainSlots();

	void CheckEquipSlot(UEquipmentItemEquipWidget* CheckWidget);

	void CheckQuickSlot();

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 CurrentIndex = 0;

	void MoveIndex(int32 Index);

	void SetIndexHover(int32 Index);
	void SetIndexUnHover(int32 Index);
	
	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);

	FSKGameplayMessageListenerHandle InteractionHandle;

	void OnInteractionMessageReceived(FGameplayTag Channel, const FUIInteractionMoveMessage& Message);

	UPROPERTY()
	TArray<UEquipmentItemBaseWidget*> SlotList;

	UPROPERTY()
	TArray<FIntPoint> SlotPositions;
	
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
