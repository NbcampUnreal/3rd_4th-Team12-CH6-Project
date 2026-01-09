#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Blocked.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Blocked::USK_GA_AI_Blocked()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Blocked")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.ShortGroggy")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.ShortGroggy")));
}

void USK_GA_AI_Blocked::Blocked(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				NAME_None,
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Blocked::OnBlockedCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Blocked::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Blocked::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Blocked::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Blocked::OnBlockedCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Blocked::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();

	AnimMontage = GetAnimMontage("Blocked");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Blocked(AnimMontage);
}

void USK_GA_AI_Blocked::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(OwnerASC))
	{
		OwnerASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Blocked")));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}