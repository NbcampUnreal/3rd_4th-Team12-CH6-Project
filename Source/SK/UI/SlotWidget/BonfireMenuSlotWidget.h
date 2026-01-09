// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "BonfireMenuSlotWidget.generated.h"

struct FUIInteractionMoveMessage;
class UButton;
/**
 * 
 */
UCLASS()
class SK_API UBonfireMenuSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/** 위젯에 직접 붙은 버튼들 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* BtnLevelUp;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* BtnLeave;

	/** 자동으로 구성되는 버튼 배열 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TArray<UButton*> MenuButtons;

	/** 현재 선택된 버튼 인덱스 */
	int32 CurrentIndex = 0;

	/** 입력 처리 */
	void MoveSelectionUp();
	void MoveSelectionDown();
	void ApplyFocusToButton(int32 OldIndex, int32 NewIndex);	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
	FButtonStyle NormalStyle;

	UPROPERTY()
	FButtonStyle HoveredStyle;
	
	FSKGameplayMessageListenerHandle MenuMoveHandle;

	void OnMenuMoveMessageReceived(FGameplayTag Channel, const FUIInteractionMoveMessage& Message);

	UFUNCTION()
	void PressCurrentButton();
	
	UFUNCTION()
	void OnLevelUpClicked();

	UFUNCTION()
	void OnLeaveClicked();
	UFUNCTION()
	void OnLeaveHovered();

	UFUNCTION()
	void SimulateButtonHover(UButton* Button);

	UFUNCTION()
	void SimulateButtonUnHover(UButton* Button);
};