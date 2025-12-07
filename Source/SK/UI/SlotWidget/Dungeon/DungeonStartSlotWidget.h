// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "DungeonStartSlotWidget.generated.h"

class UImage;
class UTextBlock;
struct FGameplayTag;
struct FLoadingUIVisible;
/**
 * 
 */
UCLASS()
class SK_API UDungeonStartSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 위젯 초기화 시 실행
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 애니메이션 재생 */
	void PlayAppearAnimation();
	
	FSKGameplayMessageListenerHandle DungeonStartHandle;

	void OnDungeonStartMessageReceived(FGameplayTag Channel, const FLoadingUIVisible& Message);
	
protected:

	/** 클리어 텍스트 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapText;

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
