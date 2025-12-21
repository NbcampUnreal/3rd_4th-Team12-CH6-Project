// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"   
#include "SKCameraShakeBase.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKCameraShakeBase : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	
//	USKCameraShakeBase();
	USKCameraShakeBase(const FObjectInitializer& ObjectInitializer);



	static float G_YawAmp;
	static float G_PitchAmp;
	static float G_Frequency;
	static float G_Duration;

	
};
