// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SlotWidget/Equipment/EquipmentItemBaseWidget.h"
#include "EquipmentItemQuickWidget.generated.h"

enum class EEquipmentSlotType : uint8;
enum class EInventoryItemType : uint8;
/**
 * 
 */
UCLASS()
class SK_API UEquipmentItemQuickWidget : public UEquipmentItemBaseWidget
{
	GENERATED_BODY()
public:
	void SettingItem(int32 ItemID);

	void SettingSlot(EInventoryItemType ItemType, int32 QuickSlotNumber, UInventoryComponent* InventoryComponent);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentQuickWidget")
	EInventoryItemType CurrentItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentQuickWidget")
	int32 CurrentQuickSlotNumber;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="EquipmentQuickWidget")
	UInventoryComponent* CurrentInventoryComponent;
	
	virtual void OnClicked() override;	
};
