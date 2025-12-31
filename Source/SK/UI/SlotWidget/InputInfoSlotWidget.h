// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "InputInfoSlotWidget.generated.h"

struct FSlotVisibilityMessage;
/**
 * 
 */
UCLASS()
class SK_API UInputInfoSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* SlotImage;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	FSKGameplayMessageListenerHandle SlotVisibleHandle;

	void OnSlotVisibleMessageReceived(FGameplayTag Channel, const FSlotVisibilityMessage& Message);

	
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CloseAnim;
};
