// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/InGameLayoutWidget.h"

#include "AbilitySystemComponent.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UInGameLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InGameMenuHandle = RegisterUIActionBinding(FBindUIActionArgs(InGameInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleInGameAction)));
}

void UInGameLayoutWidget::HandleInGameAction()
{
	CachedASC->AddLooseGameplayTag(TAG_UI_Layout_EscapeMenu);
}
