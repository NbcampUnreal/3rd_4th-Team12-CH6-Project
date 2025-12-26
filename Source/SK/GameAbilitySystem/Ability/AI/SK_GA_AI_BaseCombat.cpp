#include "GameAbilitySystem/Ability/AI/SK_GA_AI_BaseCombat.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Anim/AI/SK_AnimNotify_AI_SendEventToASC.h"
#include "Controller/AI/SKAIController.h"
#include "GameFramework/Character.h"

USK_GA_AI_BaseCombat::USK_GA_AI_BaseCombat()
{
	
}

void USK_GA_AI_BaseCombat::ApplyDamageToTarget(TWeakObjectPtr<const AActor> TargetActor)
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

	//추가
	FGameplayEffectSpec* Spec = DamageSpecHandle.Data.Get();
	if (!Spec)
	{
		return;
	}

	// 공격 타입 태그 전달
	if (CurrentAttackType.IsValid())
	{
		Spec->DynamicGrantedTags.AddTag(CurrentAttackType);
	}
	
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

TObjectPtr<AActor> USK_GA_AI_BaseCombat::GetTargetActor() const
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		return nullptr;
	}

	TObjectPtr<AActor> TargetActor = AIController->GetTargetActor();

	return TargetActor;
}

FVector USK_GA_AI_BaseCombat::GetPredictedTargetLocation(float PredictionTime) const
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		return CachedCharacter->GetActorLocation();
	}

	FVector PredictedTargetLocation = AIController->GetPredictedTargetLocation(PredictionTime);
	
	return PredictedTargetLocation;
}

FVector USK_GA_AI_BaseCombat::GetPredictedToTargetDirection(const FVector& PredictedLocation) const
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		return CachedCharacter->GetActorForwardVector();
	}

	FVector PredictedToTargetDirection = AIController->GetPredictedToTargetDirection(PredictedLocation);

	return PredictedToTargetDirection;
}

float USK_GA_AI_BaseCombat::GetRushTime(const UAnimMontage& LocalAnimMontage) const
{
	if (!IsValid(&LocalAnimMontage))
	{
		return 0.f;
	}

	float StartTime = -1.f;
	float EndTime = -1.f;
	
	for (const FAnimNotifyEvent& NotifyEvent : LocalAnimMontage.Notifies)
	{
		USK_AnimNotify_AI_SendEventToASC* AnimNotify = Cast<USK_AnimNotify_AI_SendEventToASC>(NotifyEvent.Notify);
		if (!IsValid(AnimNotify))
		{
			continue;
		}
		
		if (AnimNotify->GetName() == "Start")
		{
			StartTime = NotifyEvent.GetTriggerTime();
		}
		else if (AnimNotify->GetName() == "End")
		{
			EndTime = NotifyEvent.GetTriggerTime();
		}
	}

	if (StartTime >= 0.f && EndTime >= 0.f)
	{
		return (EndTime - StartTime);
	}
	
	return 0.f;	
}

void USK_GA_AI_BaseCombat::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USK_GA_AI_BaseCombat::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}