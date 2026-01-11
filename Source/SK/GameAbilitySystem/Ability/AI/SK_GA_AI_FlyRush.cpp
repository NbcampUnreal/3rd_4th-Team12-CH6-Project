#include "GameAbilitySystem/Ability/AI/SK_GA_AI_FlyRush.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AI/SKAIBaseAnimInstance.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USK_GA_AI_FlyRush::USK_GA_AI_FlyRush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.FlyRush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.FlyRush")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.FlyRush")));
}

void USK_GA_AI_FlyRush::FlyRush(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	CachedCharacter->bUseControllerRotationPitch = true;
	
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_FlyRush::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();
	
	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_FlyRush::OnHitCompleted);
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
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_FlyRush::OnFlyRushCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_FlyRush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_FlyRush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_FlyRush::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_FlyRush::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	if (!IsValid(AnimMontage))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	CurrentAttackType = EventData.EventTag;
	AddDamageMultiplierByAttackType();
	DamageMultiplier += 0.4f;
	
	AActor* TargetActor = GetTargetActor();
	if (!IsValid(TargetActor))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	FVector Offset = FVector(150.f, 0.f, 0.f);
	float RushTime = GetRushTime(*AnimMontage);
	
	OwnFlyRushTask = UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(
				this,
				"FlyRush",
				TargetActor,
				Offset,
				ERootMotionMoveToActorTargetOffsetType::AlignFromTargetToSource,
				RushTime,
				nullptr,
				nullptr,
				true,
				MOVE_Flying,
				true,
				nullptr,
				nullptr,
				ERootMotionFinishVelocityMode::SetVelocity,
				FVector::ZeroVector,
				0.f,
				false
				);
	OwnFlyRushTask->ReadyForActivation();
}

void USK_GA_AI_FlyRush::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_FlyRush::OnFlyRushCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_FlyRush::ActivateAbility(
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
	
	int32 MaxFlyRushIndex = AICharacter->GetMaxFlyRushIndex();
	
	if (MaxFlyRushIndex > 1)
	{
		int32 FlyRushIndex = FMath::RandRange(1, MaxFlyRushIndex);
		FString AnimMontageName = FString::Printf(TEXT("FlyRush%d"), FlyRushIndex);
		AnimMontage = GetAnimMontage(*AnimMontageName);
	}
	else
	{
		AnimMontage = GetAnimMontage("FlyRush1");
	}
	
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FlyRush(AnimMontage);
}

void USK_GA_AI_FlyRush::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	CachedCharacter->bUseControllerRotationPitch = false;

	ClearFocus();
	
	FRotator CurrentRot = CachedCharacter->GetActorRotation();
	CurrentRot.Pitch = 0.f; 
	CurrentRot.Roll = 0.f;
	CachedCharacter->SetActorRotation(CurrentRot);

	USKAIBaseAnimInstance* AIAnimInstance =  Cast<USKAIBaseAnimInstance>(CachedCharacter->GetMesh()->GetAnimInstance());
	if (IsValid(AIAnimInstance))
	{
		AIAnimInstance->SetbIsFlying(false);
		CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
