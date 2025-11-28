// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "ItemAddInfoWidget.generated.h"

class UTextBlock;
struct FItemAddMessage;
/**
 * 
 */
UCLASS()
class SK_API UItemAddInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SettingViewItem(const FText& ItemName, int32 ItemCount, UTexture2D* InItemIcon);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 텍스트 및 이미지 바인딩
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIconImage;

	/** 팝업 Fade 애니메이션 */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* PopupFadeAnimation;

	UFUNCTION(BlueprintCallable)
	void PlayViewAnimation();
};
