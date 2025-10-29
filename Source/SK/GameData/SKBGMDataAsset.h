// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKBGMLevelData.h"
#include "SKBGMDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKBGMDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="BGM")		
	TArray<FSKBGMData> BGMData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	float VolumeMultiplier = 1.0f;								

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	float FadeTime = 1.0f;		
};
