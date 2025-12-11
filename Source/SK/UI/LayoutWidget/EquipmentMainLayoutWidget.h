// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SKLayoutWidgetBase.h"
#include "EquipmentMainLayoutWidget.generated.h"

class UButton;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle EquipMainToInGameData2;
	
	FUIActionBindingHandle EquipMainToInGameHandle2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MoveUpData;
	
	FUIActionBindingHandle MoveUpHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MoveDownData;
    	
	FUIActionBindingHandle MoveDownHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MoveLeftData;
	
	FUIActionBindingHandle MoveLeftHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle MoveRightData;
	
	FUIActionBindingHandle MoveRightHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FDataTableRowHandle SelectionData;
	
	FUIActionBindingHandle SelectionHandle;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleEquipMainToInGameAction();

	UFUNCTION()
	void HandleMoveUpAction();

	UFUNCTION()
	void HandleMoveDownAction();

	UFUNCTION()
	void HandleMoveLeftAction();

	UFUNCTION()
	void HandleMoveRightAction();

	UFUNCTION()
	void HandleSelectionAction();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
	USoundBase* ClickSound;

	UFUNCTION()
	void HandleCloseButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
};
