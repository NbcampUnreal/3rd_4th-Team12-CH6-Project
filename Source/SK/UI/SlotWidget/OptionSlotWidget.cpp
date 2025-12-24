// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/OptionSlotWidget.h"

#include "Components/Slider.h"
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
