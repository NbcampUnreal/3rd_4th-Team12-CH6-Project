// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraShakeBase.h"

float USKCameraShakeBase::G_YawAmp     = 6.f;
float USKCameraShakeBase::G_PitchAmp  = 4.f;
float USKCameraShakeBase::G_Frequency = 12.f;
float USKCameraShakeBase::G_Duration  = 0.15f;

// USKCameraShakeBase::USKCameraShakeBase()
// {
// 	UPerlinNoiseCameraShakePattern* Pattern =
// 		CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(TEXT("Perlin"));
//
// 	Pattern->Yaw.Amplitude   = G_YawAmp;
// 	Pattern->Yaw.Frequency  = G_Frequency;
//
// 	Pattern->Pitch.Amplitude = G_PitchAmp;
// 	Pattern->Pitch.Frequency = G_Frequency;
//
// 	Pattern->Roll.Amplitude  = 0.f;
// 	Pattern->Roll.Frequency = 0.f;
//
// 	Pattern->Duration = G_Duration;
//
// 	SetRootShakePattern(Pattern);
// }

USKCameraShakeBase::USKCameraShakeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)   
{
	UPerlinNoiseCameraShakePattern* Pattern =
	CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(TEXT("Perlin"));

	Pattern->Yaw.Amplitude   = G_YawAmp;
	Pattern->Yaw.Frequency  = G_Frequency;

	Pattern->Pitch.Amplitude = G_PitchAmp;
	Pattern->Pitch.Frequency = G_Frequency;

	Pattern->Roll.Amplitude  = 0.f;
	Pattern->Roll.Frequency = 0.f;

	Pattern->Duration = G_Duration;

	SetRootShakePattern(Pattern);
}
