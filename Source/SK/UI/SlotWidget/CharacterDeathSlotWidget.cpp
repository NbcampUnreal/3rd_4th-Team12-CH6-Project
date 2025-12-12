// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/CharacterDeathSlotWidget.h"

#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UCharacterDeathSlotWidget::PlayDeathSequence()
{
	if (!DeathAnim)
	{
		UE_LOG(LogTemp, Error, TEXT("[DeathWidget] DeathAnim 없음"));
		return;
	}

	// 애니메이션 실행
	UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] Death Sequence 재생"));
	PlayAnimation(DeathAnim);
}

void UCharacterDeathSlotWidget::OnPlayerDieHandleMessageReceived(FGameplayTag Channel,
	const FSlotVisibilityMessage& Message)
{
	if (Message.SlotTags.HasTag(TAG_UI_Slot_CharacterDeath) && Message.bVisible)
	{
		PlayDeathSequence();
	}
}

void UCharacterDeathSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] NativeConstruct"));

	if (DeathAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &UCharacterDeathSlotWidget::OnDeathAnimationFinished);

		BindToAnimationFinished(DeathAnim, EndEvent);
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		return;
	}

	PlayerDieHandle = MessageSubsystem->RegisterListener<FSlotVisibilityMessage>(
		TAG_Message_Channel_SlotVisible,
		this,
		&UCharacterDeathSlotWidget::OnPlayerDieHandleMessageReceived
	);
}

void UCharacterDeathSlotWidget::NativeDestruct()
{
	if (PlayerDieHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(PlayerDieHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UCharacterDeathSlotWidget::OnDeathAnimationFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] 애니메이션 종료 → 위젯 제거"));
	if (UWorld* InnerWorld = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(InnerWorld))
		{
			FSlotVisibilityMessage SlotMessage;
			SlotMessage.LayoutTag = TAG_UI_Layout_InGame;
			SlotMessage.SlotTags.AddTag(TAG_UI_Slot_CharacterDeath);
			SlotMessage.bVisible = false;

			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, SlotMessage);
		}
	}
}
