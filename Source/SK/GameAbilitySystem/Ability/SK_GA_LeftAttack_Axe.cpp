// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/SKCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_LeftAttack_Axe::USK_GA_LeftAttack_Axe()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void USK_GA_LeftAttack_Axe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	if (!IsValid(SKPlayerState))
		return;

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);

	PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, true);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	UAnimInstance* BaseAnim = Character->GetMesh()->GetAnimInstance();
	if (!IsValid(BaseAnim))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	USKPlayerAnimInstance* PlayerAnimInstance = Cast<USKPlayerAnimInstance>(BaseAnim);
	if (!IsValid(PlayerAnimInstance))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
}

void USK_GA_LeftAttack_Axe::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);

	PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	PlayerCharacter->SetLooseTag(TAG_State_Action_ATK, false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USK_GA_LeftAttack_Axe::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}


bool USK_GA_LeftAttack_Axe::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      FGameplayTagContainer* OptionalRelevantTags) const
{
	bool result = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (!result)
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character))
			return result;

		ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);

		PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	}

	return result;
}

void USK_GA_LeftAttack_Axe::OnMontageCompleted()
{
	UE_LOG(LogTemp, Error, TEXT("[GA] Montage Completed"));


	// Ability 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_LeftAttack_Axe::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Error, TEXT("[GA] Montage Interrupted"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}


void USK_GA_LeftAttack_Axe::ApplyDamageFromTrace()
{
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;

	
}

void USK_GA_LeftAttack_Axe::OnStopAttackTrace_Server()
{
	ApplyDamageFromTrace();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
