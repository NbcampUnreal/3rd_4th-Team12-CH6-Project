// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusSlotWidget.generated.h"

struct FGameplayEffectSpec;
class UProgressBar;
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
	
protected:
	UPROPERTY()
	const USKAttributeSet* AttributeSet;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;
	
	void PossessPawnChanged(APawn* ChangePawn);
	
	void HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);
};
