// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "EquipSelectListSlotWidget.generated.h"

class UEquipSelectItemWidget;
class UEquipmentComponent;
class UQuickSlotComponent;
class UScrollBox;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class SK_API UEquipSelectListSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
protected:
	void TryCachedComponent();

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UEquipSelectItemWidget> ItemWidgetClass;
	
	UPROPERTY()
	TArray<UEquipSelectItemWidget*> ItemWidgetPool;
	
	UPROPERTY(meta=(BindWidget))
	UScrollBox* InventoryScroll;
	
	UPROPERTY()
	UInventoryComponent* CachedInventory;

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY()
	UEquipmentComponent* CachedEquipment;

	UPROPERTY()
	EInventoryItemType CurrentItemType;

	UPROPERTY()
	EEquipmentSlotType CurrentEquipmentSlotType;

	UPROPERTY()
	int32 CurrentQuickSlotNumber;
	
	FSKGameplayMessageListenerHandle ItemSwitchHandle;

	// 한 줄에 몇 칸
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 ItemsPerRow = 5;

	// 최소 슬롯 개수
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 MinSlotCount = 20;
	
	void OnItemSwitchMessageReceived(FGameplayTag Channel, const FItemSwitchMessage& Message);
	
	void RefreshInventory();
};
