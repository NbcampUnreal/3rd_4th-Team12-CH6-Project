// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKSoundDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FSKSoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SoundTag;  // 예: Sound.SFX.Attack

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> Sound;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoop = false;
};

UCLASS()
class USKSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	TArray<FSKSoundData> SoundList;
};