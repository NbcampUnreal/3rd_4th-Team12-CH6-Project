// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusSlotWidget.generated.h"

class UImage;
struct FGameplayEffectSpec;
class UProgressBar;
class UHorizontalBox;
class USKAttributeSet;
/**
 * 
 */
UCLASS()
class SK_API UCharacterStatusSlotWidget : public UUserWidget
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
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HeatContainer;

	UPROPERTY()
	TArray<UProgressBar*> HeatBars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Heat")
	UTexture2D* FullHeatTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Heat")
	UTexture2D* EmptyHeatTexture;
	
	void HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const;
	
	void StaminaChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const;

	void HeatChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const;

	void MaxHeatChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

};
