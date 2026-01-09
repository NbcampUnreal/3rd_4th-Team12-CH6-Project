#include "GameAbilitySystem/Ability/AI/SK_GA_AI_HideRush.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/RootMotionSource.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_AI_HideRush::USK_GA_AI_HideRush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.HideRush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HideRush")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HideRush")));
}

void USK_GA_AI_HideRush::HideRush(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_HideRush::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();
	
	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_HideRush::OnHitCompleted);
	OwnEventTask2->ReadyForActivation();

	OwnEventTask3 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("AI.StartHide")),
				nullptr,
				true,
				false
				);
	OwnEventTask3->EventReceived.AddDynamic(this, &USK_GA_AI_HideRush::OnAnimNotifyCompleted);
	OwnEventTask3->ReadyForActivation();

	OwnEventTask4 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("AI.EndHide")),
				nullptr,
				true,
				false
				);
	OwnEventTask4->EventReceived.AddDynamic(this, &USK_GA_AI_HideRush::OnAnimNotifyCompleted);
	OwnEventTask4->ReadyForActivation();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				"Default",
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_HideRush::OnHideRushCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_HideRush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_HideRush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_HideRush::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_HideRush::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	if (EventData.EventTag.MatchesTag(TAG_Attack))
	{
		if (!IsValid(AnimMontage))
		{
			EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
			return;
		}
	
		CurrentAttackType = EventData.EventTag;
		AddDamageMultiplierByAttackType();
		DamageMultiplier += 0.3f;
	
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
	else if (EventData.EventTag == TAG_AI_StartHide)
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		if (!IsValid(SourceASC))
		{
			return;
		}
		
		FVector SpawnLocation = CachedCharacter->GetActorLocation();
		float AICapsuleHalfHeight = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SpawnLocation.Z -= AICapsuleHalfHeight;
	
		FGameplayCueParameters Params;
		Params.Location = SpawnLocation;

		SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));
		SourceASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.AI.Teleport"), Params);
	
		CachedCharacter->SetActorHiddenInGame(true);
	}
	else if (EventData.EventTag == TAG_AI_EndHide)
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		if (!IsValid(SourceASC))
		{
			return;
		}
	
		FVector SpawnLocation = CachedCharacter->GetActorLocation();
		float AICapsuleHalfHeight = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SpawnLocation.Z -= AICapsuleHalfHeight;
	
		FGameplayCueParameters Params;
		Params.Location = SpawnLocation;

		SourceASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.AI.Teleport"), Params);
		SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));

		CachedCharacter->SetActorHiddenInGame(false);
	}
}

void USK_GA_AI_HideRush::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_HideRush::OnHideRushCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_HideRush::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	/*
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	int32 MaxHideRushIndex = AICharacter->GetMaxHideRushIndex();
	
	if (MaxHideRushIndex > 1)
	{
		int32 RushIndex = FMath::RandRange(1, MaxHideRushIndex);
		FString AnimMontageName = FString::Printf(TEXT("HideRush%d"), RushIndex);
		AnimMontage = GetAnimMontage(*AnimMontageName);
	}
	else
	{
		AnimMontage = GetAnimMontage("HideRush1");
	}
	*/

	AnimMontage = GetAnimMontage("HideRush1");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	HideRush(AnimMontage);
}

void USK_GA_AI_HideRush::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(SourceASC))
	{
		if (SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide")))
		{
			SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}