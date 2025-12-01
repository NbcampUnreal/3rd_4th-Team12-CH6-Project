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

void UCharacterDeathSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] NativeConstruct"));

	if (DeathAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &UCharacterDeathSlotWidget::OnDeathAnimationFinished);

		BindToAnimationFinished(DeathAnim, EndEvent);

		UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] DeathAnim 애니메이션 종료 델리게이트 바인딩 완료"));

		if (GetWorld())
		{
			FTimerHandle TestHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TestHandle,
				FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					UE_LOG(LogTemp, Warning, TEXT("[DeathWidget] 3초 후 DeathAnim 테스트 재생"));
					if (UWorld* InnerWorld = GetWorld())
					{
						if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(InnerWorld))
						{
							FSlotVisibilityMessage SlotMessage;
							SlotMessage.LayoutTag = TAG_UI_Layout_InGame;
							SlotMessage.SlotTags.AddTag(TAG_UI_Slot_CharacterDeath);
							SlotMessage.bVisible = true;

							MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, SlotMessage);

						}
					}
					PlayDeathSequence();
				}),
				3.0f,
				false
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DeathWidget] DeathAnim 없음!!"));
	}
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
