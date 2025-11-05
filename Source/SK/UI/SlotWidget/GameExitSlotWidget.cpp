// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GameExitSlotWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/SKUIManagerSubSystem.h"

void UGameExitSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UGameExitSlotWidget::OnContinueClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UGameExitSlotWidget::OnExitClicked);
	}
}

void UGameExitSlotWidget::HandleRequestResult(bool bResult)
{
	if (!bIsRequestConfirm)
		return;

	if (bResult)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			UKismetSystemLibrary::QuitGame(
				this,       // WorldContextObject
				PC,         // PlayerController
				EQuitPreference::Quit,  // 종료 옵션
				true        // bIgnorePlatformRestrictions
			);
		}
	}
}

void UGameExitSlotWidget::OnContinueClicked()
{
	UAbilitySystemComponent* ASC = CurrentUISubSystem->GetCachedASC();
	if (!ASC)
		return;

	ASC->AddLooseGameplayTag(TAG_UI_Layout_InGame);
}

void UGameExitSlotWidget::OnExitClicked()
{
	ConfirmRequest();
}


