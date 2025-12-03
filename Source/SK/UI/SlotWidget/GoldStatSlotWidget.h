// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoldStatSlotWidget.generated.h"

struct FGameplayEffectSpec;
class UTextBlock;
class USKAttributeSet;
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
	const USKAttributeSet* AttributeSet;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalGoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlusGoldText;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* GoldAddAnimation;

	mutable float PendingGold = 0.0f;
	mutable bool bIsPlayingAnimation = false;
	
	void GlodChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	void OnGoldAddAnimationFinished();
};
