#include "GameAbilitySystem/Ability/AI/SK_GA_AI_BaseCombat.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
	SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
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