#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Ready.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Ready::USK_GA_AI_Ready()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Ready")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ready")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ready")));
}

void USK_GA_AI_Ready::Ready(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				NAME_None,
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Ready::OnReadyCompleted);
	//Task->OnInterrupted.AddDynamic(this, &USK_GA_Melee::OnMontageInterrupted);
	//Task->OnCancelled.AddDynamic(this, &USK_GA_Melee::OnMontageCancelled);
	//Task->OnBlendOut.AddDynamic(this, &USK_GA_Melee::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Ready::OnReadyCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Ready::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	AnimMontage = GetAnimMontage("Ready");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Ready(AnimMontage);
}

void USK_GA_AI_Ready::EndAbility(
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
