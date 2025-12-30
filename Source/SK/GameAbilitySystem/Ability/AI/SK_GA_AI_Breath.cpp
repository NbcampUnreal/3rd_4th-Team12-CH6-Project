#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Breath.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Breath::USK_GA_AI_Breath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Breath")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Breath")));
}

void USK_GA_AI_Breath::Breath(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Breath::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();

	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				"Default",
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Breath::OnBreathCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Breath::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Breath::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Breath::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Breath::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	
}

void USK_GA_AI_Breath::OnBreathCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Breath::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();
	
	AnimMontage = GetAnimMontage("Breath");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Breath(AnimMontage);
}

void USK_GA_AI_Breath::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}