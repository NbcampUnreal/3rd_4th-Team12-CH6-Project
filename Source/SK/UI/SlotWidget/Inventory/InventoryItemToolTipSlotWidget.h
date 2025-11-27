// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "InventoryItemToolTipSlotWidget.generated.h"

class USKInventoryItemData;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UInventoryItemToolTipSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Inventory|Tooltip")
	void SetItemData(USKInventoryItemData* ItemData);
 
protected:
	// 위젯에서 BindWidget으로 연결할 변수들
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;
 
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;
 
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemDescriptionText;
 
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemRarityText;
 
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemTypeText;
 
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ItemStatText;

	FSKGameplayMessageListenerHandle ToolTipSwitchHandle;

	void OnToolTipSwitchMessageReceived(FGameplayTag Channel, const FToolTipSwitch& Message);
	
	// 내부에서 장비/소모품 등 타입별로 툴팁 추가 정보 갱신 함수
	void UpdateAdditionalStats(USKInventoryItemData* ItemData);

	virtual void NativeConstruct() override;

	
};
