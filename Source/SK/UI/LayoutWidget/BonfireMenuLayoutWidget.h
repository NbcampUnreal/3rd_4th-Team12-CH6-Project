// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "BonfireMenuLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UBonfireMenuLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuToInGameData;
	
	FUIActionBindingHandle MenuToInGameHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuUPData;
	
	FUIActionBindingHandle MenuUPHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuDownData;
	
	FUIActionBindingHandle MenuDownHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MenuSelectData;
	
	FUIActionBindingHandle MenuSelectHandle;
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleMenuToInGameAction();

	UFUNCTION()
	void HandleMeneUpAction();

	UFUNCTION()
	void HandleMenuDownAction();

	UFUNCTION()
	void HandleMenuSelectAction();
};
