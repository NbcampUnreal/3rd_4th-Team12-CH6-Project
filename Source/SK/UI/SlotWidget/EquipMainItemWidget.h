// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipMainItemWidget.generated.h"

class UEquipmentComponent;
class UEquipmentInstance;
enum class EEquipmentSlotType : uint8;
enum class EInventoryItemType : uint8;
class UImage;

USTRUCT(BlueprintType)
struct FEquipMainItemForWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentItemID;
	
	UPROPERTY(BlueprintReadWrite)
	UEquipmentInstance* CurrentEquipInstance;
};
/**
 * 
 */
UCLASS()
class SK_API UEquipMainItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetItem(const int32 ItemID, UEquipmentComponent* EquipmentComponent);

	UFUNCTION(BlueprintCallable)
	void SettingSlot(EInventoryItemType ItemType, EEquipmentSlotType SlotType, int32 QuickSlotNumber);
	
	FEquipMainItemForWidget CurrentEquipMainItem;

	UPROPERTY(BlueprintReadWrite)
	EEquipmentSlotType CurrentEquipSlotType;
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadWrite)
	EInventoryItemType CurrentItemType;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentQuickSlotNumber;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
 
	// Blueprint에서 바인딩할 수 있는 이벤트 함수도 선언 가능
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemHovered();
 
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemUnhovered();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemClicked();	
};
