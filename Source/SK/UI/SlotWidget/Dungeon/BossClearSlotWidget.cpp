// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Dungeon/BossClearSlotWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UBossClearSlotWidget::NativeConstruct()
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
	BossClearHandle = MessageSubsystem->RegisterListener<FSlotVisibilityMessage>(
		TAG_Message_Channel_SlotVisible,
		this,
		&UBossClearSlotWidget::OnBossClearMessageReceived
	);
}

void UBossClearSlotWidget::NativeDestruct()
{
	if (BossClearHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(BossClearHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UBossClearSlotWidget::PlayAppearAnimation()
{
	if (AppearAnim)
	{
		PlayAnimation(AppearAnim);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UBossClearSlotWidget::PlayAppearSound,
			0.5f,
			false
		);
	}
}

void UBossClearSlotWidget::OnBossClearMessageReceived(FGameplayTag Channel, const FSlotVisibilityMessage& Message)
{
	if (Message.SlotTags.HasTag(TAG_UI_Slot_BossHP))
	{
		PlayAppearAnimation();
	}
}

void UBossClearSlotWidget::PlayAppearSound()
{
	if (AppearSound)
	{
		UGameplayStatics::PlaySound2D(this, AppearSound);
	}
}
