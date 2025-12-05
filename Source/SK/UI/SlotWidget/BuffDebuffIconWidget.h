// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/StaticData/BuffDebuffIconDataTable.h"
#include "BuffDebuffIconWidget.generated.h"

struct FModifiedAttributeInfo;
class UProgressBar;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class SK_API UBuffDebuffIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetupIcon(
		const FBuffDebuffIconDataTable& Data,
		const FModifiedAttributeInfo& Info,
		float Duration,
		int32 StackCount,
		bool bSettingIsBuff
	);

	void StartDurationTimer(float Duration);
	void UpdateDuration();
	void SetDuration(float NewDuration);

	float TotalDuration = 0.f;

	// Remaining duration
	float RemainingDuration = 0.f;

	// Timer for ticking duration
	FTimerHandle DurationTimerHandle;

	bool bIsBuff;

	void SetStackCount(int32 NewStackCount);
	
	/** 기본 아이콘 (어떤 스탯인지) */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* StatIcon;

	/** 증가/감소 아이콘 (ex: ↑ / ↓) */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* ChangeIcon;

	/** 수치 +% 등 텍스트 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* ValueText;

	/** 스택 수 표시 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* StackText;

	/** 남은 시간 ProgressBar */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UProgressBar* DurationProgressBar;

	/** 남은 시간 텍스트 (ex: 12.3s) */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* DurationText;
};
