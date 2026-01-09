// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/OptionSlotWidget.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/VerticalBox.h"
#include "GameInstance/SKGameInstance.h"

void UOptionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 초기 볼륨 값 세팅
	InitializeVolumeValues();

	// 슬라이더 바인딩
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(
			this, &UOptionSlotWidget::OnMasterVolumeChanged);
	}

	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(
			this, &UOptionSlotWidget::OnBGMVolumeChanged);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(
			this, &UOptionSlotWidget::OnSFXVolumeChanged);
	}

	if (SoundOptionButton)
	{
		SoundOptionButton->OnClicked.AddDynamic(this, &UOptionSlotWidget::OnSoundOptionClicked);
	}

	if (GraphicOptionButton)
	{
		GraphicOptionButton->OnClicked.AddDynamic(this, &UOptionSlotWidget::OnGraphicOptionClicked);
	}

	// 기본 상태 (사운드 먼저 보여주고 싶다면)
	SetOptionBoxVisibility(SoundOptionBox);
}

void UOptionSlotWidget::OnMasterVolumeChanged(float Value)
{
	if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
	{
		GI->SetMasterVolume(Value);
	}
}

void UOptionSlotWidget::OnBGMVolumeChanged(float Value)
{
	if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
	{
		GI->SetBGMVolume(Value);
	}
}

void UOptionSlotWidget::OnSFXVolumeChanged(float Value)
{
	if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
	{
		GI->SetSFXVolume(Value);
	}
}

void UOptionSlotWidget::OnSoundOptionClicked()
{
	SetOptionBoxVisibility(SoundOptionBox);
}

void UOptionSlotWidget::OnGraphicOptionClicked()
{
	SetOptionBoxVisibility(GraphicOptionBox);
}

void UOptionSlotWidget::SetOptionBoxVisibility(UVerticalBox* TargetBox)
{
	if (SoundOptionBox)
	{
		SoundOptionBox->SetVisibility(
			TargetBox == SoundOptionBox
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);
	}

	if (GraphicOptionBox)
	{
		GraphicOptionBox->SetVisibility(
			TargetBox == GraphicOptionBox
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);
	}
}

void UOptionSlotWidget::InitializeVolumeValues()
{
	if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
	{
		if (MasterVolumeSlider)
		{
			MasterVolumeSlider->SetValue(GI->GetMasterVolume());
		}

		if (BGMVolumeSlider)
		{
			BGMVolumeSlider->SetValue(GI->GetBGMVolume());
		}

		if (SFXVolumeSlider)
		{
			SFXVolumeSlider->SetValue(GI->GetSFXVolume());
		}
	}
}
