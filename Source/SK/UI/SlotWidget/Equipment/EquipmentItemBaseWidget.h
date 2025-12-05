// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentItemBaseWidget.generated.h"

class UImage;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class SK_API UEquipmentItemBaseWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	int32 GetItemID() const { return CurrentItemID; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* ClickSound;
	
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EquipmentItemSlotWidget")
	int32 CurrentItemID = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EquipmentItemSlotWidget")
	UInventoryComponent* CachedInventory;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SendHoverMessage(bool bHover) const;
	virtual void OnClicked() {}

	void PlayUISound(USoundBase* InSound);
};
