#include "GameAbilitySystem/Ability/AI/SK_GA_AI_HitReaction.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

USK_GA_AI_HitReaction::USK_GA_AI_HitReaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.HitReaction")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
}

void USK_GA_AI_HitReaction::HitReaction(TObjectPtr<UAnimMontage> AnimMontage)
{
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.0f,
				NAME_None,
				false,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_HitReaction::OnHitReactionCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_HitReaction::OnHitReactionCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_HitReaction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();

	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("HitReaction");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	HitReaction(AnimMontage);
}

void USK_GA_AI_HitReaction::EndAbility(
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
		if (OwnerASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction"))))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("태그제거"));
			OwnerASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
