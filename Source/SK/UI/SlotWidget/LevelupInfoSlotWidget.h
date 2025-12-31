// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "LevelupInfoSlotWidget.generated.h"

class ASKPlayerState;
class UOverlay;
class UTextBlock;
struct FSwitchLayoutMessage;
/**
 * 
 */
UCLASS()
class SK_API ULevelupInfoSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);

	void TryBind();
	void RetryLater(const TCHAR* Reason);

	FTimerHandle RetryBindTimer;
	int32 RetryCount = 0;
	static const int32 MaxRetryCount = 5;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* CurrentLevelText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Stat")
	UTextBlock* NextLevelText;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Level")
	UTextBlock* RequireGold;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Level")
	UTextBlock* UpStatMaxHealth;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Level")
	UTextBlock* UpStatMaxStamina;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Level")
	UTextBlock* UpStatAttack;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category="Level")
	UTextBlock* UpStatArmor;

	UPROPERTY()
	ASKPlayerState* CurrentPS;
	
	
	void SettingLevelUpDataByLevel(int32 Level);
};
