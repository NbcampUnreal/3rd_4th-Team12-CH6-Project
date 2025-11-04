// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmSlotWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class SK_API UConfirmSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "SlotWidget")
	void SettingText(FConfirmUITextRow TextData);
	
protected:
	virtual void NativeConstruct() override;
	
	// 블루프린트에서 연결할 수 있는 버튼 2개
	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	// 블루프린트에서 연결할 수 있는 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ConfirmButtonText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CancelButtonText;

	UPROPERTY()
	FGameplayTag RequestSlotTag;
	
	// 버튼 클릭 처리 함수
	UFUNCTION()
	void HandleConfirmClicked();

	UFUNCTION()
	void HandleCancelClicked();
};
