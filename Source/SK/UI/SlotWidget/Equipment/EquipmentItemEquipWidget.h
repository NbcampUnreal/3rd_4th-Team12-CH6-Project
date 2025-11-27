// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SlotWidget/Equipment/EquipmentItemBaseWidget.h"
#include "EquipmentItemEquipWidget.generated.h"

enum class EEquipmentSlotType : uint8;
enum class EInventoryItemType : uint8;
class UEquipmentInstance;
/**
 * 
 */
UCLASS()
class SK_API UEquipmentItemEquipWidget : public UEquipmentItemBaseWidget
{
	GENERATED_BODY()
public:
	void SettingItem(int32 ItemID, UEquipmentInstance* ItemInstance);

	void SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, UInventoryComponent* InventoryComponent);

	EEquipmentSlotType GetSlotType() const { return CurrentSlotType; }

	UEquipmentInstance* GetCurrentInstance() const {return CurrentEquipInstance;}
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentEquipWidget")
	UEquipmentInstance* CurrentEquipInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentEquipWidget")
	EEquipmentSlotType CurrentSlotType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentEquipWidget")
	EInventoryItemType CurrentItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentEquipWidget")
	UInventoryComponent* CurrentInventoryComponent;
	
	virtual void OnClicked() override;
};
