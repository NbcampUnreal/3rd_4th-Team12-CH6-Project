// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthWidget.generated.h"

class UProgressBar;
struct FGameplayEffectSpec;
class USKAIAttributeSet;
/**
 * 
 */
UCLASS()
class SK_API UMonsterHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SettingWidget(APawn* OwnerPawn);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	void HideHealthBar();
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY()
	const USKAIAttributeSet* AttributeSet;

	FTimerHandle HideTimerHandle;


};
