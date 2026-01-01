// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "CharacterStatSlotWidget.generated.h"

class ASKPlayerState;
class USKAttributeSet;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UCharacterStatSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void TryBind();
	void RetryLater(const TCHAR* Reason);

	FTimerHandle RetryBindTimer;
	int32 RetryCount = 0;
	static const int32 MaxRetryCount = 5;
	
	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);
		
protected:
	/** 최대 생명력 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* MaxHealthText;

	/** 최대 기력 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* MaxStaminaText;

	/** 공격력 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* AttackPowerText;

	/** 방어력 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* DefensePowerText;

	/** 피해 감소율 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* DamageReductionText;

	/** 레벨 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* LevelStringText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat")
	const USKAttributeSet* CharacterStat;
	
	UPROPERTY()
	ASKPlayerState* CurrentPS;
	
	UFUNCTION()
	void ReflashStat();

	UFUNCTION()
	void SetStatText(UTextBlock* Text, float Value);
};
