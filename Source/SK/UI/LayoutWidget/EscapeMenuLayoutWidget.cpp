// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EscapeMenuLayoutWidget.h"

#include "AbilitySystemComponent.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputTypes.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UEscapeMenuLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EscapeMenuHandle = RegisterUIActionBinding(FBindUIActionArgs(EscapeMenuInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeMenuAction)));
}

void UEscapeMenuLayoutWidget::HandleEscapeMenuAction()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage SwitchMessage(TAG_UI_Layout_InGame, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, SwitchMessage);

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *SwitchMessage.LayoutTag.ToString());

			FSlotVisibilityMessage VisibilityMessage;
			VisibilityMessage.LayoutTag = TAG_UI_Layout_InGame;
			VisibilityMessage.SlotTags.AddTag(TAG_UI_Slot_CharacterStatus);
			VisibilityMessage.bVisible = true;
			VisibilityMessage.VisibleDuration = 3.0f;

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible,	VisibilityMessage);
		}
	}
}
