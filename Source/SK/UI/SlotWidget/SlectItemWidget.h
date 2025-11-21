// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/InventoryComponent.h"
#include "SlectItemWidget.generated.h"

class UTextBlock;
class UImage;
struct FInventorySlot;
class UEquipmentComponent;
enum class EInventoryItemType : uint8;
enum class EEquipmentSlotType : uint8;

/**
 * 
 */
UCLASS()
class SK_API USlectItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetItem(FInventorySlot InvenSlot);

	UFUNCTION(BlueprintCallable)
	void SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, int32 QuickSlotNum);
	
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

	UPROPERTY(BlueprintReadWrite)
	EInventoryItemType CurrentItemType;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
 
	// Blueprint에서 바인딩할 수 있는 이벤트 함수도 선언 가능
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemHovered();
 
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemUnhovered();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemLeftClicked();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemRightClicked();
};
