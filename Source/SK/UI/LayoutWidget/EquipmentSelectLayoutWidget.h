// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "EquipmentSelectLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UEquipmentSelectLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle SelectToMainActionData;
	
	FUIActionBindingHandle SelectToMainHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleSelectToMainAction();
};
