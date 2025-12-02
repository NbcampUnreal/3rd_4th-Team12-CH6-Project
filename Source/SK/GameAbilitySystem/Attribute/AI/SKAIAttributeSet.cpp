#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

USKAIAttributeSet::USKAIAttributeSet()
{
	InitHealth(300.f);
	InitMaxHealth(300.f);
	InitAttack(20.f);
	InitArmor(10.f);
	InitPoise(20.f);
	InitSpeed(400.f);
	InitSprintWeight(1.5f);
}

void USKAIAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USKAIAttributeSet, SprintWeight, COND_None, REPNOTIFY_Always);
}

void USKAIAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void USKAIAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (FMath::IsNearlyZero(GetHealth()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Death"));

			UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();
			if (!OwningASC)
			{
				return;
			}
			
			FGameplayEventData EventData;
			EventData.Instigator = nullptr;
			EventData.Target = nullptr;
			EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Death"));
			EventData.OptionalObject = nullptr;

			OwningASC->HandleGameplayEvent(EventData.EventTag, &EventData);
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("죽음이벤트발송"));

			/* 보스 죽었을 때 처리 로직에 추가
			if (USKGameplayMessageSubsystem* MessageSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USKGameplayMessageSubsystem>())
			{
				FSlotVisibilityMessage Message;

				Message.LayoutTag = TAG_UI_Layout_InGame;
				Message.SlotTags.AddTag(TAG_UI_Slot_BossHP);
				Message.bVisible = false;
				MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, Message);
			}
			*/
		}
		
		FString DebugMsg = FString::Printf(TEXT("Health: %.2f"), GetHealth());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMsg);
	}
}

void USKAIAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 서버권한 로직 처리용 ex) 몬스터 체력이 50% 이하가 되면 모든 플레이어 이속 감소라든가 등
	if (Attribute == GetHealthAttribute())
	{
		OnCurrentHealthChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			NewValue - OldValue,
			OldValue,
			NewValue
		);
	}
	else if (Attribute == GetPoiseAttribute())
	{
		OnCurrentPoiseChanged.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			NewValue - OldValue,
			OldValue,
			NewValue
		);
	}
}

void USKAIAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, Health, OldHealth);

	// 클라이언트 UI 업데이트용
	OnCurrentHealthChanged.Broadcast(
		nullptr,
		nullptr,
		nullptr,
		GetHealth() - OldHealth.GetCurrentValue(),
		OldHealth.GetCurrentValue(),
		GetHealth()
	);
}

void USKAIAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, MaxHealth, OldMaxHealth);
}

void USKAIAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, Attack, OldAttack);
}

void USKAIAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, Armor, OldArmor);
}

void USKAIAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, Poise, OldPoise);
}

void USKAIAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, Speed, OldSpeed);
}

void USKAIAttributeSet::OnRep_SprintWeight(const FGameplayAttributeData& OldSprintWeight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USKAIAttributeSet, SprintWeight, OldSprintWeight);
}