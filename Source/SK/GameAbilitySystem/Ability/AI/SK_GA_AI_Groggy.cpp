#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Groggy.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Groggy::USK_GA_AI_Groggy()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Groggy")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Groggy")));
}

void USK_GA_AI_Groggy::Groggy(TObjectPtr<UAnimMontage> LocalAnimMontage)
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
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Groggy::OnGroggyCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Groggy::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Groggy::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Groggy::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Groggy::OnGroggyCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Groggy::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();

	AnimMontage = GetAnimMontage("Groggy");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Groggy(AnimMontage);
}

void USK_GA_AI_Groggy::EndAbility(
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
		OwnerASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Groggy")));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

