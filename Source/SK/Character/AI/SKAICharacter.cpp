#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

ASKAICharacter::ASKAICharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
}

void ASKAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSetAndAbilitiesFromDataAsset();
	
	/* 보스한테 추가할 로직
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (UWorld* InnerWorld = GetWorld())
			{
				if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(InnerWorld))
				{
					FText BossName = FText::FromString(TEXT("테스트 메시지 보스 이름 텍스트"));
					FSettingBossHPWidget Message(this, BossName);

					MessageSubsystem->BroadcastMessage(TAG_Message_Channel_BossWidgetSetting, Message);
					
					FSlotVisibilityMessage SlotMessage;
					SlotMessage.LayoutTag = TAG_UI_Layout_InGame;
					SlotMessage.SlotTags.AddTag(TAG_UI_Slot_BossHP);
					SlotMessage.bVisible = true;

					MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, SlotMessage);

				}
			}
		});
	}
	*/
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

}
