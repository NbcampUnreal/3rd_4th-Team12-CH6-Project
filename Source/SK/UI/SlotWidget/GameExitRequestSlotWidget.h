// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "GameExitRequestSlotWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class SK_API UGameExitRequestSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// 위젯 생성 시 바인딩
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DungeonExitButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY()
	int32 RequestButton = 0;
	
	// 버튼 클릭 함수
	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnDungeonExitClicked();
	
	UFUNCTION()
	void OnExitClicked();
	
	FSKGameplayMessageListenerHandle ConfirmResponseHandle;

	void OnConfirmResponseMessageReceived(FGameplayTag Channel, const FConfirmResponseMessage& Message);
};
