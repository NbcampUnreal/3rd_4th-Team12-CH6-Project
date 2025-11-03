// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "EscapeMenuLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UEscapeMenuLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle EscapeMenuInputActionData;
	
	FUIActionBindingHandle EscapeMenuHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleEscapeMenuAction();
};
