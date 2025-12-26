// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "DialogueLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UDialogueLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle ToInGameDataEscape;
	
	FUIActionBindingHandle ToInGameHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle ToInGameDataClick;
	
	FUIActionBindingHandle ToInGameClickHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleToInGameAction();

	UFUNCTION()
	void HandleToInGameClickAction();
};
