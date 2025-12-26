// Fill out your copyright notice in the Description page of Project Settings.



#include "GameAbilitySystem/Ability/SKGameplayAbility.h"

USKGameplayAbility::USKGameplayAbility()
{
}

void USKGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USKGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USKGameplayAbility::ApplyDamageFromTrace()
{
	
}

void USKGameplayAbility::OnStopAttackTrace_Server()
{
	//몽타주 1번에 여러 공격이 들어갈 때 데미지 빨리 처리 시 불리는 함수
	ApplyDamageFromTrace();
}

void USKGameplayAbility::AddAttackTypeToEffectSpec(FGameplayEffectSpec& Spec) const
{
	if (AttackTypeTag.IsValid())
	{
		Spec.DynamicGrantedTags.AddTag(AttackTypeTag);
	}
}




