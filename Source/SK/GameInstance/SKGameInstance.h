// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameData/SKSoundDataAsset.h"
#include "SKGameInstance.generated.h"

class UPrimaryDataAsset;
/**
 * 
 */
UCLASS()
class SK_API USKGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// virtual void Init() override;
	// virtual void Shutdown() override;
	//
	// void HandleLevelChanged(const FName& LevelName);

#pragma region SOUND
	
	const TObjectPtr<USKSoundDataAsset>& GetSoundDataAsset();
	
	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetMasterVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetBGMVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "SK|Sound|Volume")
	void SetSFXVolume(float InVolume);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio")
	TObjectPtr<USKSoundDataAsset> SoundDataAsset;

	//종합 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float MasterVolume = 1.f;

	//브금 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float BGMVolume = 1.f;

	//UI 사운드 & 상호작용,이펙트 크기
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SK|Audio|Volume")
	float SFXVolume = 1.f;

	#pragma endregion

	//임시. 추후 데이터 에셋 매니저 나오면 변경
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPrimaryDataAsset> TestLayoutData;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPrimaryDataAsset> TestConfirmData;
};
