#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Melee.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/Character.h"

USK_GA_AI_Melee::USK_GA_AI_Melee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Melee")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Action.Melee")));
}

void USK_GA_AI_Melee::ApplyDamageToTarget(TWeakObjectPtr<const AActor> TargetActor)
{
	AActor* Target = const_cast<AActor*>(TargetActor.Get());
	if (!IsValid(Target))
	{
		return;
	}

	if (DamageEffectClasses.IsEmpty())
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return;
	}
	
	UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		return;	
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	FGameplayEffectSpecHandle DamageSpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClasses[0], // 임시
		GetAbilityLevel(),
		ContextHandle
	);
	if (!DamageSpecHandle.IsValid())
	{
		return;
	}
	/*
	DamageSpecHandle.Data->SetSetByCallerMagnitude(
			GASProjectTags::SetByCaller_Damage,
			-BaseDamage
		);
	*/
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

void USK_GA_AI_Melee::Melee(UAnimMontage* AnimMontage)
{
	UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				true,
				false
			);

	EventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Melee::OnHitCompleted);
	EventTask->ReadyForActivation();
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AnimMontage,
			1.0f,
			NAME_None,
			false,
			1.0f
		);

	MontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Melee::OnMeleeCompleted);
	//Task->OnInterrupted.AddDynamic(this, &USK_GA_Melee::OnMontageInterrupted);
	//Task->OnCancelled.AddDynamic(this, &USK_GA_Melee::OnMontageCancelled);
	//Task->OnBlendOut.AddDynamic(this, &USK_GA_Melee::OnMontageBlendOut);
	MontageTask->ReadyForActivation();
}

void USK_GA_AI_Melee::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_Melee::OnMeleeCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Melee::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* AnimMontage = AICharacter->GetMontages()[0]; // 임시로 일단 0번 인덱스 고정
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Melee(AnimMontage);
}

void USK_GA_AI_Melee::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
