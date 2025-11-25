// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterDamageWidget.generated.h"

class USKAIAttributeSet;
class UTextBlock;
struct FGameplayEffectSpec;
/**
 * 
 */
UCLASS()
class SK_API UMonsterDamageWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SettingWidget(APawn* OwnerPawn);
	
protected:
	virtual void NativeConstruct() override;

	void HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	void HideDamageText();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY()
	const USKAIAttributeSet* AttributeSet;

	FTimerHandle HideTimerHandle;
};
