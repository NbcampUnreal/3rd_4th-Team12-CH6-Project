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
}

void USK_GA_LeftAttack_Axe::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// if (!ActorInfo->IsNetAuthority() && !ActorInfo->IsLocallyControlled())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("[GA] Blocked SimulatedProxy execution"));
	// 	return;
	// }
	
	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	if (!IsValid(SKPlayerState))
		return;

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);
	USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
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
	
	CombatComponent->Server_IncreaseComboIndex(true);

	int32 ComboIndex = CombatComponent->GetComboIndex();
	int32 MaxIndex = LeftAttackDamageGE.Num() - 1;
	int32 SafeIndex = FMath::Clamp(ComboIndex - 1, 0, MaxIndex);
	
	UAnimMontage* Montage = CombatComponent->GetLeftAttackMontage(0);
	FName SectionName = FName(*FString::Printf(TEXT("Combo_%02d"), SafeIndex + 1));

	if (PlayerAnimInstance->Montage_IsPlaying(Montage))
	{
		PlayerAnimInstance->Montage_Stop(0.1f, Montage);
	}
	UAbilityTask_PlayMontageAndWait* Task =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Montage,
		1.f
		,SectionName
		,false
	);

	Task->OnCompleted.AddDynamic(this, &USK_GA_LeftAttack_Axe::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &USK_GA_LeftAttack_Axe::OnMontageInterrupted);
	Task->ReadyForActivation();


	// 클라이언트 전용
	// if (ActorInfo->IsLocallyControlled())
	// {
	// 	UAnimInstance* AIM = Character->GetMesh()->GetAnimInstance();
	// 	AIM->Montage_Play(Montage, 1.f);
	// 	AIM->Montage_JumpToSection(SectionName, Montage);
	// }

	// CombatComponent->Client_PlayMontage(Montage, SectionName);

	UE_LOG(LogTemp, Error, TEXT("GA 시작시 AnimInstance = %s"),
		ActorInfo->AnimInstance.IsValid() ? *ActorInfo->AnimInstance->GetName() : TEXT("NULL"));
	
		const FGameplayAbilityActorInfo* AI = ASC->AbilityActorInfo.Get();

		UAnimInstance* Anim = AI && AI->AnimInstance.IsValid() 
			? AI->AnimInstance.Get() 
			: nullptr;

		UE_LOG(LogTemp, Error, TEXT("ASC AvatarActor = %s"),
			AI && AI->AvatarActor.IsValid() ? *AI->AvatarActor->GetName() : TEXT("NULL"));

		UE_LOG(LogTemp, Error, TEXT("ASC OwnerActor = %s"),
			AI && AI->OwnerActor.IsValid() ? *AI->OwnerActor->GetName() : TEXT("NULL"));

		UE_LOG(LogTemp, Error, TEXT("ASC AnimInstance = %s"),
			Anim ? *Anim->GetName() : TEXT("NULL"));
	
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
	USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();

	// CombatComponent->Server_OnATKEndNotify_Implementation(true);
	// CombatComponent->StopMontage_Local(0.2f);
	//
	// if (GetOwningActorFromActorInfo()->HasAuthority())
	// {
	// 	CombatComponent->Multicast_StopMontage(0.2f);
	// }

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void USK_GA_LeftAttack_Axe::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
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
		USKCombatComponent* CombatComponent = PlayerCharacter->GetCombatComponent();
		CombatComponent->ResetComboState();
		PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	}

	return result;
}

void USK_GA_LeftAttack_Axe::OnMontageCompleted()
{
	UE_LOG(LogTemp, Error, TEXT("[GA] Montage Completed"));

	// // 공격 끝났으므로 EndNotify 호출
	// ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	// if (PC)
	// {
	// 	USKCombatComponent* Combat = PC->GetCombatComponent();
	// 	if (Combat)
	// 	{
	// 		Combat->Server_OnATKEndNotify_Implementation(true);
	// 	}
	// }

	// Ability 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_LeftAttack_Axe::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Error, TEXT("[GA] Montage Interrupted"));

	// 공격 콤보 상태 리셋
	// ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	// if (PC)
	// {
	// 	USKCombatComponent* Combat = PC->GetCombatComponent();
	// 	if (Combat)
	// 	{
	// 		Combat->ResetComboState();
	// 	}
	// }

	// Ability 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}


void USK_GA_LeftAttack_Axe::ApplyDamageFromTrace()
{
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;
	USKCombatComponent* CombatComponent = PC->GetCombatComponent();
	int LeftATKIndex = CombatComponent->GetComboIndex();
	int32 MaxIndex = LeftAttackDamageGE.Num() - 1;
	int32 SafeIndex = FMath::Clamp(LeftATKIndex, 0, MaxIndex);

	for (AActor* HitActor : CombatComponent->GetHitActors())
	{
		if (!HitActor)
			continue;

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
