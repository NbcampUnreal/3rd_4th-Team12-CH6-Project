#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Chase.h"
#include "AbilitySystemComponent.h"
#include "SK_GA_AI_Wander.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Controller/AI/SKAIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USK_GA_AI_Chase::USK_GA_AI_Chase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Chase")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Action.Melee")));
}

void USK_GA_AI_Chase::Chase()
{
	WaitMoveComplete();

	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	TObjectPtr<AActor> TargetActor = AIController->GetTargetActor();
	if (!IsValid(TargetActor))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	AIController->MoveToActor(TargetActor);
}

void USK_GA_AI_Chase::WaitMoveComplete()
{
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.MoveComplete")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Chase::OnWaitMoveCompleteCompleted);
	OwnEventTask1->ReadyForActivation();
}

void USK_GA_AI_Chase::OnWaitMoveCompleteCompleted(FGameplayEventData EventData)
{
	CommonEventTask->EndTask();
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Chase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Combat"))))
	{
		bSpeedUp = true;
		CachedCharacter->GetCharacterMovement()->MaxWalkSpeed *= 1.5f;
	}
	
	Chase();
}

void USK_GA_AI_Chase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	if (bSpeedUp)
	{
		bSpeedUp = false;
		CachedCharacter->GetCharacterMovement()->MaxWalkSpeed /= 1.5f;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}