// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/ConfirmUITextDataTable.h"
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
};
