// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SlotWidget/ConfirmRequestWidgetBase.h"
#include "GameExitSlotWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class SK_API UGameExitSlotWidget : public UConfirmRequestWidgetBase
{
	GENERATED_BODY()
protected:
	void HandleRequestResult(bool bResult) override;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	// 버튼 클릭 함수
	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnExitClicked();

	// 위젯 생성 시 바인딩
	virtual void NativeConstruct() override;
};
