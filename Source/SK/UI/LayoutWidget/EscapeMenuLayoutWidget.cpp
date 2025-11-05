// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EscapeMenuLayoutWidget.h"

#include "AbilitySystemComponent.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEscapeMenuLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EscapeMenuHandle = RegisterUIActionBinding(FBindUIActionArgs(EscapeMenuInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeMenuAction)));
}

void UEscapeMenuLayoutWidget::HandleEscapeMenuAction()
{
	CachedASC->AddLooseGameplayTag(TAG_UI_Layout_InGame);
}
