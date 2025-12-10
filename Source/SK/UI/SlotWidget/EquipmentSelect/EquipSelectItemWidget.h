// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/InventoryComponent.h"
#include "EquipSelectItemWidget.generated.h"

class UEquipSelectListSlotWidget;
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

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemLeftClicked();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemRightClicked();

	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void HoverImageVisible(bool bVisible);
	
	UPROPERTY()
	UEquipSelectListSlotWidget* ParentWidget = nullptr;

	UPROPERTY()
	int32 WidgetIndex = -1;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemQuantityText;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta=(BindWidget))
	UImage* HoverImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* SelectSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* UnSelectSound;
	
	UPROPERTY()
	UInventoryComponent* CachedInventory;

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY()
	UEquipmentComponent* CachedEquipment;
	
	UPROPERTY(BlueprintReadWrite)
	EInventoryItemType CurrentItemType;
	
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
 
	// Blueprint에서 바인딩할 수 있는 이벤트 함수도 선언 가능
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void SendHoverMessage(bool bHover);
	
	

	void PlayUISound(USoundBase* InSound);
};
