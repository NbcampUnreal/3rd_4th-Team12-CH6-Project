// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionSlotWidget.generated.h"

class UButton;
class UVerticalBox;
class USlider;
/**
 * 
 */
UCLASS()
class SK_API UOptionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	/* === Sliders === */

	UPROPERTY(meta = (BindWidget))
	USlider* MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* BGMVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* SFXVolumeSlider;
	
	/* === Callbacks === */

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SoundOptionBox;

	UPROPERTY(meta = (BindWidget))
	UButton* SoundOptionButton;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* GraphicOptionBox;

	UPROPERTY(meta = (BindWidget))
	UButton* GraphicOptionButton;

	UFUNCTION()
	void OnSoundOptionClicked();

	UFUNCTION()
	void OnGraphicOptionClicked();

	void SetOptionBoxVisibility(UVerticalBox* TargetBox);
	/* 초기 값 동기화 */
	void InitializeVolumeValues();	
};
