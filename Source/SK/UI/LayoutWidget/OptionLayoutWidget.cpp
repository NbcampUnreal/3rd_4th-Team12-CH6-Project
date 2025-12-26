// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/OptionLayoutWidget.h"

#include "Components/Button.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UOptionLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ToEscapeMenuHandle = RegisterUIActionBinding(FBindUIActionArgs(ToEscapeMenuData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleToEscapeMenu)));

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear(); // 혹시 중복 방지
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
}

void UOptionLayoutWidget::HandleToEscapeMenu()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_EscapeMenu, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);
		}
	}
}

void UOptionLayoutWidget::HandleCloseButtonClicked()
{
	HandleToEscapeMenu();
}
