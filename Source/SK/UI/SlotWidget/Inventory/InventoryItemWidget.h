// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UTextBlock;
class UImage;

USTRUCT(BlueprintType)
struct FInventoryItemForWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(BlueprintReadWrite)
	int32 Quantity;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemID;
};

/**
 * 
 */
UCLASS()
class SK_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetItem(const FInventoryItemForWidget& NewItem);

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemQuantityText;

	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* HoverSound;
		
	FInventoryItemForWidget CurrentItem;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
 
	// Blueprint에서 바인딩할 수 있는 이벤트 함수도 선언 가능
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemHovered();
 
	UFUNCTION(BlueprintCallable, Category="Inventory|Item")
	void OnItemUnhovered();

	void PlayUISound(USoundBase* InSound);
};
