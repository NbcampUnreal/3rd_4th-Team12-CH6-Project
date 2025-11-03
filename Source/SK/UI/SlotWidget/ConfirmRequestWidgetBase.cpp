// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ConfirmRequestWidgetBase.h"

#include "Utility/SKUIManagerSubSystem.h"

void UConfirmRequestWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	bIsRequestConfirm = false;
	
	ULocalPlayer* lp = GetOwningLocalPlayer();
	if (!lp)
		return;

	CurrentUISubSystem = lp->GetSubsystem<USKUIManagerSubSystem>();

	if (!CurrentUISubSystem)
		return;

	CurrentUISubSystem->OnConfirmResult.AddDynamic(this, &UConfirmRequestWidgetBase::HandleRequestResult);
}

void UConfirmRequestWidgetBase::ConfirmRequest()
{
	if (!CurrentUISubSystem)
		return;

	CurrentUISubSystem->RequestConfirmUI(ConfirmRequestData);
	bIsRequestConfirm = true;
}

void UConfirmRequestWidgetBase::HandleRequestResult(bool bResult)
{
	if (!bIsRequestConfirm)
		return;

	if (bResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("확인 창 결과: 확인"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("확인 창 결과: 취소"));
	}

	bIsRequestConfirm = false;
}
