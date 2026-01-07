#include "GameAbilitySystem/Ability/AI/SK_GA_AI_JumpRush.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/Character.h"

USK_GA_AI_JumpRush::USK_GA_AI_JumpRush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.JumpRush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.JumpRush")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.JumpRush")));
}

void USK_GA_AI_JumpRush::JumpRush(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	/*
	if (WarpDistance > MaxDistance)
	{
		PredictedHorizontalVector = PredictedHorizontalVector.GetSafeNormal() * MaxDistance;
   
		PredictedLocation = AILocation + FVector(
						PredictedHorizontalVector.X, 
						PredictedHorizontalVector.Y, 
						PredictedVector.Z 
		);
	}
	*/
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_JumpRush::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();

	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_JumpRush::OnHitCompleted);
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
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_JumpRush::OnJumpRushCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_JumpRush::OnAnimNotifyCompleted(FGameplayEventData EventData)
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
	FVector PredictedToTargetVector = GetPredictedToTargetDirection(PredictedTargetLocation);

	float Distance = FVector::Dist(CachedCharacter->GetActorLocation(), PredictedTargetLocation);
	float Height = 70.f; // 몬스터, 공격마다 다르게 적용되도록 바꿔야 함.
	
	OwnJumpRushTask = UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(
				this,
				"JumpRush",
				PredictedToTargetVector.Rotation(),
				Distance,
				Height,
				PredictionTime,
				0.1,
				true,
				ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
				FVector::ZeroVector,
				0.f,
				nullptr,
				nullptr
				);
	OwnJumpRushTask->ReadyForActivation();
}

void USK_GA_AI_JumpRush::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_JumpRush::OnJumpRushCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_JumpRush::ActivateAbility(
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
	
	int32 MaxJumpRushIndex = AICharacter->GetMaxJumpRushIndex();
	
	if (MaxJumpRushIndex > 1)
	{
		int32 JumpRushIndex = FMath::RandRange(1, MaxJumpRushIndex);
		FString AnimMontageName = FString::Printf(TEXT("JumpRush%d"), JumpRushIndex);
		AnimMontage = GetAnimMontage(*AnimMontageName);
	}
	else
	{
		AnimMontage = GetAnimMontage("JumpRush1");
	}
	
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	JumpRush(AnimMontage);
}

void USK_GA_AI_JumpRush::EndAbility(
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