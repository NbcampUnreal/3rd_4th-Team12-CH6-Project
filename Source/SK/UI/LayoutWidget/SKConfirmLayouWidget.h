// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "SKConfirmLayouWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKConfirmLayouWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackInputActionData;

	FUIActionBindingHandle BackHandle;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle MoveLeftData;

	FUIActionBindingHandle MoveLeftHandle;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle MoveRightData;

	FUIActionBindingHandle MoveRightHandle;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle SelectionData;

	FUIActionBindingHandle SelectionHandle;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void BackHandleAction();

	UFUNCTION()
	void MoveLeftHandleAction();

	UFUNCTION()
	void MoveRightHandleAction();

	UFUNCTION()
	void SelectionHandleAction();
};
