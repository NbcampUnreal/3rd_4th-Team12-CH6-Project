// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/SKGameInstance.h"
#include "Utility/SKBGMSubSystem.h"

const TObjectPtr<USKSoundDataAsset>& USKGameInstance::GetSoundDataAsset()
{
	return SoundDataAsset;
}

void USKGameInstance::SetMasterVolume(float InVolume)
{
	MasterVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);

	if (USKBGMSubSystem* BGM = this->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->UpdateVolume();
	}
}

void USKGameInstance::SetBGMVolume(float InVolume)
{
	BGMVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);

	if (USKBGMSubSystem* BGM = this->GetSubsystem<USKBGMSubSystem>())
	{
		BGM->UpdateVolume();
	}
}

void USKGameInstance::SetSFXVolume(float InVolume)
{
	SFXVolume = FMath::Clamp(InVolume, 0.0f, 1.0f);
	
}
