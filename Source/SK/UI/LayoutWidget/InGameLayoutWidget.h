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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InGameToInventoryData;
	
	FUIActionBindingHandle InGameToInventoryHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InGameToEquipMainData;
	
	FUIActionBindingHandle InGameToEquipMainHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle InputInfoActionData;
	
	FUIActionBindingHandle InputInfoHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleInGameAction();

	UFUNCTION()
	void HandleInGameToInventoryAction();

	UFUNCTION()
	void HandleInGameToEquipMainAction();

	UFUNCTION()
	void HandleInputInfoAction();

	bool bInputInfoVisible = false;
	
	UPROPERTY()
	bool bCanToggleInputInfo = true;

	UPROPERTY()
	float InputInfoCooldown = 0.5f;
};
