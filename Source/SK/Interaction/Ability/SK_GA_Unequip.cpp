// Fill out your copyright notice in the Description page of Project Settings.


#include "SK_GA_Unequip.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"

USK_GA_Unequip::USK_GA_Unequip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Action.Unequip")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Action")));
}

void USK_GA_Unequip::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	
	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	
	UAnimMontage* UnequipMontage = WeaponAnimData->UnequipMontage;
	if (!UnequipMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnequipMontage is null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	
	if (UnequipMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Unequip"), UnequipMontage);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
}

void USK_GA_Unequip::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USK_GA_Unequip::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_Unequip::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
		
	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
	if (ASC)
	{
		const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Equip"));
		// ASC->RemoveLooseGameplayTag(EquipTag);
		
		USKWeaponAnimData* AnimData = ActionComponent->GetWeaponAnimData();
		if (!AnimData)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
		
		if (AnimData->UnequipGE && AnimData->EquipGE)
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->UnequipGE, ASC, 1);
			ASC->RemoveActiveGameplayEffectBySourceEffect(AnimData->EquipGE, ASC, 1);
			
			FGameplayEffectSpecHandle UnequipGESpecHandle = ASC->MakeOutgoingSpec(AnimData->UnequipGE, 1.f, ASC->MakeEffectContext());
			if (UnequipGESpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*UnequipGESpecHandle.Data.Get());
			}
		}
	}

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	TArray<FName> UnequipSocketName = WeaponAnimData->UnequipSocketName;

	ActionComponent->AttachWeapon(UnequipSocketName);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
