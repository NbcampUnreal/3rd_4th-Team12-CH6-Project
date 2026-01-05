// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TitleWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class SK_API UTitleWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UTitleWidget();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ★ InputConfig만 적용하고 InputMode는 유지하는 오버라이드 추가
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuUPData;
	
	FUIActionBindingHandle MenuUPHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuDownData;
	
	FUIActionBindingHandle MenuDownHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuSelectData;
	
	FUIActionBindingHandle MenuSelectHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuBackData;
	
	FUIActionBindingHandle MenuBackHandle;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* OptionButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActiveInput")
	FUIInputConfig ActivaeInputConfig;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;
	
	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnFadeOutFinished();

	UFUNCTION()
	void OnFadeInFinished();
	
	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnOptionClicked();

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void HandleMeneUpAction();

	UFUNCTION()
	void HandleMenuDownAction();

	UFUNCTION()
	void HandleMenuBackAction();

	UFUNCTION()
	void HandleMenuSelectAction();

	void ApplyFocusToButton(int32 OldIndex, int32 NewIndex);

	void SimulateButtonHover(UButton* Button);
	void SimulateButtonUnHover(UButton* Button);
	void SimulateButtonPress(UButton* Button);
	void SimulateButtonRelease(UButton* Button);
	
	int32 CurrentSelectedIndex = 0;

	UPROPERTY()
	TArray<UButton*> MenuButtons;

	FTimerHandle PressedHoldTimer;
	float PressedHoldTime = 0.2f;
	
	UPROPERTY(meta = (BindWidget))
	UWidget* OptionRoot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OptionText;

	UPROPERTY(meta = (BindWidget))
	UImage* OptionImage;
	
	bool bOptionOpened = false;
	bool bCanInteract = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* MenuMoveSound;
	
	UFUNCTION()
	void PlayMenuMoveSound();
};
