// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillBarSlotWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class SK_API USkillBarSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage1;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage2;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage3;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage4;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage5;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage6;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey4;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey5;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKey6;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExtraKey1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExtraKey2;
};
