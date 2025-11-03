// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "InGameLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UInGameLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InGameInputActionData;
	
	FUIActionBindingHandle InGameMenuHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleInGameAction();
};
