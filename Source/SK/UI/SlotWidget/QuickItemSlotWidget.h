// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickItemSlotWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UQuickItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 아이템 이미지 (3개)
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage1;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage2;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage3;

	// 키 표시용 텍스트 (3개)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyText2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyText3;	
};
