// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "QuickItemSlotWidget.generated.h"

class UProgressBar;
class UInventoryComponent;
class UQuickSlotComponent;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UQuickItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 아이템 이미지 (3개)
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage1;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage2;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText3;
	
	// 키 표시용 텍스트 (3개)
	UPROPERTY(meta = (BindWidget))
	UImage* KeyImage1;

	UPROPERTY(meta = (BindWidget))
	UImage* KeyImage2;

	UPROPERTY(meta = (BindWidget))
	UImage* KeyImage3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText3;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CooldownProgressBar1;
 
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CooldownProgressBar2;
 
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CooldownProgressBar3;

	UPROPERTY()
	FSlateBrush DefaultBrush;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void TryCachedComponent();

	UFUNCTION()
	void SettingWidgetIcons();

	UPROPERTY()
	UQuickSlotComponent* CachedQuickSlot;

	UPROPERTY()
	UInventoryComponent* CachedInventory;

	FTimerHandle CooldownTimerHandles[3];
	float CooldownDurations[3] = {0.f, 0.f, 0.f};
	float CooldownElapsed[3] = {0.f, 0.f, 0.f};
 
	void StartCooldown(int32 SlotIndex, float Duration);
	void UpdateCooldownProgress(int32 SlotIndex);
	
	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);

	FSKGameplayMessageListenerHandle QuickSlotItemUseHandle;

	void OnQuickSlotItemUseMessageReceived(FGameplayTag Channel, const FQuickSlotCooldown& Message);
};
