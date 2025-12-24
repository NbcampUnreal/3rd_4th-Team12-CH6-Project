#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Melee.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"

USK_GA_AI_Melee::USK_GA_AI_Melee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Melee")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Melee")));
}

void USK_GA_AI_Melee::Melee(TObjectPtr<UAnimMontage> AnimMontage, FName StartSection)
{
	SetFocus();

	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.AnimNotify")),
				nullptr,
				false,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Melee::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();
	
	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_Melee::OnHitCompleted);
	OwnEventTask2->ReadyForActivation();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.0f,
				StartSection,
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Melee::OnMeleeCompleted);
	//Task->OnInterrupted.AddDynamic(this, &USK_GA_Melee::OnMontageInterrupted);
	//Task->OnCancelled.AddDynamic(this, &USK_GA_Melee::OnMontageCancelled);
	//Task->OnBlendOut.AddDynamic(this, &USK_GA_Melee::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Melee::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	//CurrentAttackType = ;
}

void USK_GA_AI_Melee::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_Melee::OnMeleeCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Melee::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	int32 MaxMeleeIndex = AICharacter->GetMaxMeleeIndex();
	FName SectionName = "Default";
	TObjectPtr<UAnimMontage> AnimMontage;
	
	if (MaxMeleeIndex > 1)
	{
		int32 MeleeIndex = FMath::RandRange(1, MaxMeleeIndex);
		FString AnimMontageName = FString::Printf(TEXT("Melee%d"), MeleeIndex);
		AnimMontage = GetAnimMontage(*AnimMontageName);
	}
	else
	{
		AnimMontage = GetAnimMontage("Melee1");
	}
	
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Melee(AnimMontage, SectionName);
}

void USK_GA_AI_Melee::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();

	if (OwnEventTask1)
	{
		if (OwnEventTask1->IsActive())
		{
			OwnEventTask1->EndTask();
		}
	}

	if (OwnEventTask2)
	{
		if (OwnEventTask2->IsActive())
		{
			OwnEventTask2->EndTask();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
