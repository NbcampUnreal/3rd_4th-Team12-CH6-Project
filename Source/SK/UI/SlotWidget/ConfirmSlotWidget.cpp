// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ConfirmSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameData/ConfirmUITextDataTable.h"
#include "Utility/SKUIManagerSubSystem.h"

void UConfirmSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleConfirmClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleCancelClicked);
	}
}

void UConfirmSlotWidget::HandleConfirmClicked()
{
	UE_LOG(LogTemp, Display, TEXT("ConfirmSlotWidget::HandleConfirmClicked"));
	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (!LP)
		return;
	USKUIManagerSubSystem* UISubSystem = LP->GetSubsystem<USKUIManagerSubSystem>();
	if (!UISubSystem)
		return;

	UISubSystem->RequestResult(true);
	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::HandleCancelClicked()
{
	UE_LOG(LogTemp, Display, TEXT("ConfirmSlotWidget::HandleCancelClicked"));
	UE_LOG(LogTemp, Display, TEXT("ConfirmSlotWidget::HandleConfirmClicked"));
	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (!LP)
		return;
	USKUIManagerSubSystem* UISubSystem = LP->GetSubsystem<USKUIManagerSubSystem>();
	if (!UISubSystem)
		return;

	UISubSystem->RequestResult(false);
	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::SettingText(FConfirmUITextRow TextData)
{
	if (TitleText)
	{
		TitleText->SetText(TextData.Title);
	}

	if (MessageText)
	{
		MessageText->SetText(TextData.Message);
	}

	if (ConfirmButtonText)
	{
		ConfirmButtonText->SetText(TextData.ConfirmText);
	}

	if (CancelButtonText)
	{
		CancelButtonText->SetText(TextData.CancelText);
	}

	RequestSlotTag = TextData.SlotTag;
}


