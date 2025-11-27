#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Cancel.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Cancel::USK_GA_AI_Cancel()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Cancel")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Death")));
}

void USK_GA_AI_Cancel::WaitPauseEvent()
{
	UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Pause")),
				nullptr,
				true,
				false
			);

	EventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Cancel::OnWaitPauseEventCompleted);
	EventTask->ReadyForActivation();
}

void USK_GA_AI_Cancel::OnWaitPauseEventCompleted(FGameplayEventData EventData)
{
	UAbilitySystemComponent* SourceASC = CachedActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		return;	
	}

	SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Pause")));
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Cancel::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitPauseEvent();
}

void USK_GA_AI_Cancel::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
