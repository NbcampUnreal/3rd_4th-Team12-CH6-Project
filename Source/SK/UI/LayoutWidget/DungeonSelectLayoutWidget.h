// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "DungeonSelectLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UDungeonSelectLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle ToInGameActionData;
	
	FUIActionBindingHandle ToInGameHandle;
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleToInGameAction();
};
