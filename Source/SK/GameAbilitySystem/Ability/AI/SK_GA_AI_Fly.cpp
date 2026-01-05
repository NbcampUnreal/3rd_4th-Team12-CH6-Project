#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Fly.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AI/SKAIBaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

USK_GA_AI_Fly::USK_GA_AI_Fly()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Fly")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Fly")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Fly")));
}

void USK_GA_AI_Fly::StartFly(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("AI.Fly")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Fly::OnAnimNotifyCompleted);
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
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Fly::OnStartFlyCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Fly::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Fly::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Fly::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Fly::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	if (!IsValid(AnimMontage))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	USKAIBaseAnimInstance* AIAnimInstance =  Cast<USKAIBaseAnimInstance>(CachedCharacter->GetMesh()->GetAnimInstance());
	if (!IsValid(AIAnimInstance))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	AIAnimInstance->SetbIsFlying(true);
	CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	float PredictionTime = GetRushTime(*AnimMontage);
	FVector TargetLocation = CachedCharacter->GetActorLocation();
	TargetLocation.Z = 1000.f; // 임시 하드코딩
	
	OwnRushTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
				this,
				"StartFly",
				TargetLocation,
				PredictionTime,
				true,
				MOVE_Flying,
				true,
				nullptr,
				ERootMotionFinishVelocityMode::SetVelocity,
				FVector::ZeroVector,
				0.f
				);
	OwnRushTask->ReadyForActivation();
}

void USK_GA_AI_Fly::OnStartFlyCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Fly::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();
	
	AnimMontage = GetAnimMontage("StartFly");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	StartFly(AnimMontage);
}

void USK_GA_AI_Fly::EndAbility(
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