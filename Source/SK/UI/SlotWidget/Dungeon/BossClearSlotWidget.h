// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "BossClearSlotWidget.generated.h"

struct FBossClearMessage;
class UImage;
class UTextBlock;
struct FSlotVisibilityMessage;
/**
 * 
 */
UCLASS()
class SK_API UBossClearSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 위젯 초기화 시 실행
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 애니메이션 재생 */
	void PlayAppearAnimation();
	
	FSKGameplayMessageListenerHandle BossClearHandle;

	void OnBossClearMessageReceived(FGameplayTag Channel, const FBossClearMessage& Message);
	
protected:

	/** 클리어 텍스트 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClearText;

	/** 텍스트 뒤 배경 이미지 */
	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;

	/** 등장 애니메이션 (UMG에서 Animations > 이름 입력 후 Bind) */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AppearAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound")
	USoundBase* AppearSound;

	void PlayAppearSound();
};
