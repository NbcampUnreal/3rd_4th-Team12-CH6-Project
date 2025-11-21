// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "EquipmentMainLayoutWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UEquipmentMainLayoutWidget : public USKLayoutWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle EquipMainToInGameData;
	
	FUIActionBindingHandle EquipMainToInGameHandle;
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleEquipMainToInGameAction();
};
