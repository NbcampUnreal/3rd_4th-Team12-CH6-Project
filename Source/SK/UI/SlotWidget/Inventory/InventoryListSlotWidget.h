// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "InventoryListSlotWidget.generated.h"

UENUM(BlueprintType)
enum class EInventoryFilterType : uint8
{
	All       UMETA(DisplayName = "전체"),
	Equipment UMETA(DisplayName = "장비"),
	Consumables UMETA(DisplayName = "소모품"),
	Others    UMETA(DisplayName = "기타"),
};

class UInventoryComponent;
class UScrollBox;
class UButton;
/**
 * 
 */
UCLASS()
class SK_API UInventoryListSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
protected:
	void TryCachedInventory();

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

	UPROPERTY()
	TArray<UInventoryItemWidget*> ItemWidgetPool;
	
	UPROPERTY(meta=(BindWidget))
	UScrollBox* InventoryScroll;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonAll;
 
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonEquipment;
 
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonConsumables;
 
	UPROPERTY(meta=(BindWidget))
	UButton* ButtonOthers;
	
	// 인벤토리 컴포넌트
	UPROPERTY()
	UInventoryComponent* CachedInventory;

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	EInventoryFilterType CurrentFilter = EInventoryFilterType::All;
	
	// 한 줄에 몇 칸
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 ItemsPerRow = 5;

	// 최소 슬롯 개수
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 MinSlotCount = 20;

	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);
	
	void RefreshInventory();

	// 버튼 클릭 핸들러
	UFUNCTION()
	void OnButtonAllClicked();
 
	UFUNCTION()
	void OnButtonEquipmentClicked();
 
	UFUNCTION()
	void OnButtonConsumablesClicked();
 
	UFUNCTION()
	void OnButtonOthersClicked();
};
