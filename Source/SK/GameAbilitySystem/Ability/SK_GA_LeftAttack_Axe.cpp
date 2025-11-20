// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Anim/SkAnimInstance_Axe.h"
#include "Character/SKPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

USK_GA_LeftAttack_Axe::USK_GA_LeftAttack_Axe()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

void USK_GA_LeftAttack_Axe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	UAnimInstance* BaseAnim = Character->GetMesh()->GetAnimInstance();
	if (!IsValid(BaseAnim))
		return;

	USkAnimInstance_Axe* AxeAnimInstance = Cast<USkAnimInstance_Axe>(BaseAnim);
	if (!AxeAnimInstance)
		return;


	AxeAnimInstance->PlayLeftAttackAnim();
}

void USK_GA_LeftAttack_Axe::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void USK_GA_LeftAttack_Axe::ApplyDamageFromTrace()
{
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;

	int LeftATKIndex = PC->GetIsLeftComboIndexByTag();

	for (AActor* HitActor : PC->GetHitActors())
	{
		if (!HitActor)
			continue;

		TSubclassOf<UGameplayEffect> EffectClass = LeftAttackDamageGE[LeftATKIndex];

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);

		// Target의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (SpecHandle.IsValid() && TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
