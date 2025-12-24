// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "OptionLayoutWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class SK_API UOptionLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle ToEscapeMenuData;
	
	FUIActionBindingHandle ToEscapeMenuHandle;
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleToEscapeMenu();

	UFUNCTION()
	void HandleCloseButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
};
