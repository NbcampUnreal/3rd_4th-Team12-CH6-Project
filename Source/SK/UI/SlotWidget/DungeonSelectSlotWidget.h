// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonSelectSlotWidget.generated.h"

class USizeBox;
class UButton;
/**
 * 
 */
UCLASS()
class SK_API UDungeonSelectSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
 
	// Buttons for selecting dungeons and closing the widget
	UPROPERTY(meta = (BindWidget))
	UButton* Dungeon1Button;
 
	UPROPERTY(meta = (BindWidget))
	UButton* Dungeon2Button;
 
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
 
	// SizeBoxes to hold Image/Text groups for dungeon info visibility toggling
	UPROPERTY(meta = (BindWidget))
	USizeBox* Dungeon1InfoBox;
 
	UPROPERTY(meta = (BindWidget))
	USizeBox* Dungeon2InfoBox;
 
	// Button hover event handlers
	UFUNCTION()
	void OnDungeon1Hovered();
 
	UFUNCTION()
	void OnDungeon1Unhovered();
 
	UFUNCTION()
	void OnDungeon2Hovered();
 
	UFUNCTION()
	void OnDungeon2Unhovered();
 
	// Button click event handlers
	UFUNCTION()
	void OnDungeon1Clicked();
 
	UFUNCTION()
	void OnDungeon2Clicked();
 
	UFUNCTION()
	void OnCloseClicked();
 
private:
	void SetDungeonInfoVisibility(USizeBox* InfoBox, bool bVisible);	
};
