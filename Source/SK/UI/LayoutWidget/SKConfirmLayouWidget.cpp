// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/SKConfirmLayouWidget.h"

#include "UI/SKSlotBox.h"
#include "UI/SlotWidget/ConfirmSlotWidget.h"
#include "Utility/SKNativeGameplayTags.h"

void USKConfirmLayouWidget::SettingConfirmMesseage(FName RowName)
{
	if (!ConfirmUIDataTable)
		return;

	FConfirmUITextRow* Row = ConfirmUIDataTable->FindRow<FConfirmUITextRow>(RowName, TEXT("SettingConfirmMesseage"));
	
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConfirmLayout] Row not found: %s"), *RowName.ToString());
		return;
	}
	
	USKSlotBox* ConfirmSlotBox = FindDynamicEntryBoxBySlotTag(TAG_UI_Slot_ConfirmSlot);
	if (!ConfirmSlotBox)
	{
		UE_LOG(LogTemp, Log, TEXT("[ConfirmSlotBox] No found"));
		return;
	}
		
	
	UUserWidget* CurrentWidget = ConfirmSlotBox->GetCurrentSlotWidgetFromBox();
	if (!CurrentWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConfirmSlotWidget] Current widget is null"));
		return;
	}

	UConfirmSlotWidget* ConfirmSlotWidget = Cast<UConfirmSlotWidget>(CurrentWidget);
	if (!ConfirmSlotWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ConfirmSlotWidget] Cast failed"));
		return;
	}
	
	ConfirmSlotWidget->SettingText(*Row);
}
