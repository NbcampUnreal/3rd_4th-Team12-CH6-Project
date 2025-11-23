// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_LeftAttack_Axe.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Anim/SkAnimInstance_Axe.h"
#include "Character/SKPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_LeftAttack_Axe::USK_GA_LeftAttack_Axe()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void USK_GA_LeftAttack_Axe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


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
		return;

	USkAnimInstance_Axe* AxeAnimInstance = Cast<USkAnimInstance_Axe>(BaseAnim);
	if (!AxeAnimInstance)
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();

	// int32 ComboIndex = PlayerCharacter->GetComboIndex();
	TArray<UAnimMontage*> Array_Montage = AxeAnimInstance->GetLeftATKMontage();


	ASKPlayerState* PS = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());

	PS->Server_IncreaseComboIndex(true);


	int32 ComboIndex = PS->GetComboIndex();
	int32 MaxIndex = LeftAttackDamageGE.Num() - 1;
	int32 SafeIndex = FMath::Clamp(ComboIndex - 1, 0, MaxIndex);
	// UE_LOG(LogTemp, Error,
	// 	   TEXT("[GA] ComboIndex = %d | SafeIndex = %d | MaxIndex = %d | MontageCount = %d"),
	// 	   ComboIndex,
	// 	   SafeIndex,
	// 	   MaxIndex,
	// 	   Array_Montage.Num()
	// );
	UAnimMontage* Montage = Array_Montage[SafeIndex];

	ASC->PlayMontage(
		this,
		ActivationInfo,
		Montage,
		1.0f
	);

	//  클라이언트 전용
	if (ActorInfo->IsLocallyControlled())
	{
		UAnimInstance* AIM = Character->GetMesh()->GetAnimInstance();
		if (AIM)
		{
			AIM->Montage_Play(Montage, 1.f);
		}
	}


	if (PlayerCharacter)
	{
		PlayerCharacter->Client_PlayMontage(Montage);
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

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

		PlayerCharacter->ResetComboState();
		PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	}

	return result;
}


void USK_GA_LeftAttack_Axe::ApplyDamageFromTrace()
{
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;

	ASKPlayerState* PS = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	int LeftATKIndex = PS->GetComboIndex();
	int32 MaxIndex = LeftAttackDamageGE.Num() - 1;
	int32 SafeIndex = FMath::Clamp(LeftATKIndex, 0, MaxIndex);

	for (AActor* HitActor : PC->GetHitActors())
	{
		if (!HitActor)
			continue;

		// TSubclassOf<UGameplayEffect> EffectClass = LeftAttackDamageGE[LeftATKIndex];
		TSubclassOf<UGameplayEffect> EffectClass = LeftAttackDamageGE[SafeIndex];

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);

		// Target의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (SpecHandle.IsValid() && TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void USK_GA_LeftAttack_Axe::OnStopAttackTrace_Server()
{
	ApplyDamageFromTrace();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
