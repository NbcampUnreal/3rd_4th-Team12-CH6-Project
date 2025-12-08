// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Skill_01.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/SKPlayerAnimInstance.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/SKCombatComponent.h"
#include "Utility/SKNativeGameplayTags.h"

void USK_GA_Skill_01::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	if (!IsValid(SKPlayerState))
		return;

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	UAnimInstance* BaseAnim = Character->GetMesh()->GetAnimInstance();
	if (!IsValid(BaseAnim))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	

	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);
	USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();

	
	UAnimMontage* Montage = CombatComponent->GetLeftAttackMontage(0);
	FName SectionName = FName(*FString::Printf(TEXT("Skill_00")));
	
	CombatComponent->Multicast_PlayMontage(Montage,SectionName);
	PlayerCharacter->SetLooseTag(TAG_State_Action_ATK, true);
}

void USK_GA_Skill_01::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);

	// PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_Skill, false);
	PlayerCharacter->SetLooseTag(TAG_State_Action_ATK, false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USK_GA_Skill_01::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	bool result = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (!result)
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (!IsValid(Character))
			return result;

		ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);
		USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
		CombatComponent->ResetComboState(); //스킬도이게맞을까?
		
		PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_Skill, false);
	}
	
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void USK_GA_Skill_01::ApplyDamageFromTrace()
{
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;
	USKCombatComponent* CombatComponent = PC->GetCombatComponent();
	int LeftATKIndex = CombatComponent->GetComboIndex();
	int32 MaxIndex = SkillDamageGE.Num() - 1;
	int32 SafeIndex = FMath::Clamp(LeftATKIndex, 0, MaxIndex);

	for (AActor* HitActor : CombatComponent->GetHitActors())
	{
		if (!HitActor)
			continue;

		TSubclassOf<UGameplayEffect> EffectClass = SkillDamageGE[SafeIndex];

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);

		// Target의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (SpecHandle.IsValid() && TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void USK_GA_Skill_01::OnStopAttackTrace_Server()
{
	ApplyDamageFromTrace();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
