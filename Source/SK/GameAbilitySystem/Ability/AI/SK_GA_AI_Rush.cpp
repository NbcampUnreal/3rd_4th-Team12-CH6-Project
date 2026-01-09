#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Rush.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/RootMotionSource.h"

USK_GA_AI_Rush::USK_GA_AI_Rush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Rush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Rush")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Rush")));
}

void USK_GA_AI_Rush::Rush(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Rush::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();

	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_Rush::OnHitCompleted);
	OwnEventTask2->ReadyForActivation();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				"Default",
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Rush::OnRushCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Rush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Rush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Rush::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Rush::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	if (!IsValid(AnimMontage))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	CurrentAttackType = EventData.EventTag;
	AddDamageMultiplierByAttackType();
	DamageMultiplier += 0.2f;
	
	float PredictionTime = GetRushTime(*AnimMontage);
	FVector PredictedTargetLocation = GetPredictedTargetLocation(PredictionTime);
	
	OwnRushTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
				this,
				"Rush",
				PredictedTargetLocation,
				PredictionTime,
				true,
				MOVE_Walking,
				true,
				nullptr,
				ERootMotionFinishVelocityMode::SetVelocity,
				FVector::ZeroVector,
				0.f
				);
	OwnRushTask->ReadyForActivation();
}

void USK_GA_AI_Rush::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_Rush::OnRushCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Rush::ActivateAbility(
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

	int32 MaxRushIndex = AICharacter->GetMaxRushIndex();
	
	if (MaxRushIndex > 1)
	{
		int32 RushIndex = FMath::RandRange(1, MaxRushIndex);
		FString AnimMontageName = FString::Printf(TEXT("Rush%d"), RushIndex);
		AnimMontage = GetAnimMontage(*AnimMontageName);
	}
	else
	{
		AnimMontage = GetAnimMontage("Rush1");
	}
	
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Rush(AnimMontage);
}

void USK_GA_AI_Rush::EndAbility(
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
