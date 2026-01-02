// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/InputInfoSlotWidget.h"

#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UInputInfoSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	// 메시지 구독
	SlotVisibleHandle = MessageSubsystem->RegisterListener<FSlotVisibilityMessage>(
		TAG_Message_Channel_ToolTipItem,
		this,
		&UInputInfoSlotWidget::OnSlotVisibleMessageReceived
	);
}

void UInputInfoSlotWidget::NativeDestruct()
{
	if (SlotVisibleHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(SlotVisibleHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UInputInfoSlotWidget::OnSlotVisibleMessageReceived(FGameplayTag Channel, const FSlotVisibilityMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_InGame)
		return;

	if (!Message.SlotTags.HasTag(TAG_UI_Slot_InputInfo))
		return;
	
	if (Message.bVisible)
	{
		if (!IsAnimationPlaying(OpenAnim))
		{
			if (IsAnimationPlaying(CloseAnim))
			{
				StopAnimation(CloseAnim);
			}

			PlayAnimation(OpenAnim);
		}
	}
	else
	{
		if (!IsAnimationPlaying(CloseAnim))
		{
			if (IsAnimationPlaying(OpenAnim))
			{
				StopAnimation(OpenAnim);
			}

			PlayAnimation(CloseAnim);
		}
	}
}
