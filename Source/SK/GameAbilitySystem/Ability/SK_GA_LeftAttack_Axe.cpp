// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"

#include "Anim/SkAnimInstance_Axe.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void USK_GA_LeftAttack_Axe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	UAnimInstance* BaseAnim = Character->GetMesh()->GetAnimInstance();
	if (!IsValid(BaseAnim))
		return;

	USkAnimInstance_Axe* AxeAnimInstance = Cast<USkAnimInstance_Axe>(BaseAnim);
	if (!AxeAnimInstance)
		return;


	// UAbilityTask_PlayMontageAndWait* Task =
	// UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	// 	this,
	// 	NAME_None,
	// 	AttackMontage
	// );
	//
	// Task->EventReceived.AddDynamic(this, &ThisClass::OnMontageNotifyBegin);
	// Task->Activate();
	
	AxeAnimInstance->PlayLeftAttackAnim();
	
}

void USK_GA_LeftAttack_Axe::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
