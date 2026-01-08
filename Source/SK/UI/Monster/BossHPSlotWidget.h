// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "BossHPSlotWidget.generated.h"

class UTextBlock;
struct FSettingBossHPWidget;
struct FGameplayEffectSpec;
class UProgressBar;
class USKAIAttributeSet;
/**
 * 
 */
UCLASS()
class SK_API UBossHPSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SettingWidget(APawn* OwnerPawn);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);
	void PoiseChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	FSKGameplayMessageListenerHandle SettingBossHandle;

	void OnSettingBossMessageReceived(FGameplayTag Channel, const FSettingBossHPWidget& Message);
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* TargetHealthProgressBar;
	
	UPROPERTY()
	float DisplayHealthPercent = 1.0f;

	UPROPERTY()
	float TargetHealthPercent = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="Health")
	float HealthInterpSpeed = 8.0f;
	
	FTimerHandle HealthAnimTimerHandle;

	void StartHealthAnimation();
	void AnimateHealth();
	void StopHealthAnimation();
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PoiseProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;
	
	UPROPERTY()
	const USKAIAttributeSet* AttributeSet;
};
