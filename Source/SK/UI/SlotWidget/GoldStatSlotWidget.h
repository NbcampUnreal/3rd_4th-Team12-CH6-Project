// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoldStatSlotWidget.generated.h"

class ASKPlayerState;
struct FGameplayEffectSpec;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UGoldStatSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void TryBind();
protected:
	UPROPERTY()
	ASKPlayerState* CurrentPS;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalGoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlusGoldText;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* GoldAddAnimation;

	mutable int32 PendingGold = 0;
	mutable bool bIsPlayingAnimation = false;

	UFUNCTION()
	void GoldChanged(int32 NewGold, int32 OldGold);

	UFUNCTION()
	void OnGoldAddAnimationFinished();
};
