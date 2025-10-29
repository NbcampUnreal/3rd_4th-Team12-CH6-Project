// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Sound/SoundBase.h"
#include "SKBGMLevelData.generated.h"

UENUM(BlueprintType)
enum class EBGMType : uint8
{
	None		UMETA(DisplayName = "None"),
	TestMap		UMETA(DisplayName = "TestMap"),
	MainMenu	UMETA(DisplayName = "MainMenu"),
	LobbyMap	UMETA(DisplayName = "LobbyMap"),
	Level_1		UMETA(DisplayName = "Level_1"),			
	Level_2		UMETA(DisplayName = "Level_2"),			
	Level_3		UMETA(DisplayName = "Level_3"),			
};


USTRUCT(BlueprintType)
struct FSKBGMData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	EBGMType BGMTypeName = EBGMType::None;		

	// USoundBase는 SoundWave, SoundCue 등 모든 사운드 타입을 받을 수 있음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TObjectPtr<USoundBase> BGMSound;			
};