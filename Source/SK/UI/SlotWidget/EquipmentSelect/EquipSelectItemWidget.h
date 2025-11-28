// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/InventoryComponent.h"
#include "EquipSelectItemWidget.generated.h"

class UEquipmentComponent;
class UQuickSlotComponent;
class UImage;
class UTextBlock;
enum class EEquipmentSlotType : uint8;
struct FInventorySlot;


/**
 * 
 */
UCLASS()
class SK_API UEquipSelectItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetItem(FInventorySlot SettingInventorySlot);

	UFUNCTION(BlueprintCallable)
	void SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, int32 QuickSlotNum);

	UFUNCTION(BlueprintCallable)
	void SendComponent(UQuickSlotComponent* QSComponent, UEquipmentComponent* EquipComponent, UInventoryComponent* InventoryComponent);
	
	FInventorySlot CurrentInventorySlot;

	UPROPERTY(BlueprintReadWrite)
	EEquipmentSlotType CurrentEquipSlotType;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentQuickSlotNum;
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemQuantityText;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY()
	UInventoryComponent* CachedInventory;

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY()
	UEquipmentComponent* CachedEquipment;
	
	UPROPERTY(BlueprintReadWrite)
	EInventoryItemType CurrentItemType;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
 
	// Blueprint에서 바인딩할 수 있는 이벤트 함수도 선언 가능
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void SendHoverMessage(bool bHover);
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemLeftClicked();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemRightClicked();	
};
