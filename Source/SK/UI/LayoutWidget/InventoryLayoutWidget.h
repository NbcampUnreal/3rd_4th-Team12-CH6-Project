// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "InventoryLayoutWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class SK_API UInventoryLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InventoryToInGameData;
	
	FUIActionBindingHandle InventoryToInGameHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InventoryToEscapeData;
	
	FUIActionBindingHandle InventoryToEscapeHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleInventoryToInGameAction();

	UFUNCTION()
	void HandleInventoryEscapeAction();

	UFUNCTION()
	void HandleCloseButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
};
