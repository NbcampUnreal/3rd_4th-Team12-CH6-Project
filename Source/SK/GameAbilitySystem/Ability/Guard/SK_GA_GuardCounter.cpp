// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/Guard/SK_GA_GuardCounter.h"
#include "AbilitySystemComponent.h"
#include "Utility/SKNativeGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "Component/BattleComponent.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USK_GA_GuardCounter::USK_GA_GuardCounter()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FGameplayTagContainer(TAG_Ability_GuardCounter));

	// 발동 조건
	ActivationRequiredTags.AddTag(TAG_State_Action_Guard_CounterReady);

	// Guard 중에는 Counter 외 행동 차단
	//ActivationBlockedTags.AddTag(TAG_State_Action_Guard);

	AttackTypeTag = TAG_Attack_Heavy;
}

void USK_GA_GuardCounter::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USKActionComponent* ActionComponent = Character->GetActionComponent();
	if (!ActionComponent)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	GuardCounterMontage = WeaponAnimData->GuardCounterMontage;
	if (!GuardCounterMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// CounterReady는 1회성
	ASC->RemoveLooseGameplayTag(TAG_State_Action_Guard_CounterReady);
	// Guard 상태 해제 (반격으로 전환)
	ASC->RemoveLooseGameplayTag(TAG_State_Action_Guard);

	UAbilityTask_PlayMontageAndWait* PlayTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			GuardCounterMontage,
			1.0f
		);

	if (GuardCounterMontage)
	{
		PlayTask->OnCompleted.AddDynamic(this, &USK_GA_GuardCounter::OnMontageFinished);
		PlayTask->OnInterrupted.AddDynamic(this, &USK_GA_GuardCounter::OnMontageFinished);
		PlayTask->OnCancelled.AddDynamic(this, &USK_GA_GuardCounter::OnMontageFinished);
		PlayTask->OnBlendOut.AddDynamic(this, &USK_GA_GuardCounter::OnMontageFinished);
		PlayTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	//ASC->ExecuteGameplayCue(TAG_Cue_Guard_Counter);
}

void USK_GA_GuardCounter::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USK_GA_GuardCounter::ApplyDamageFromTrace()
{
	Super::ApplyDamageFromTrace();

	UE_LOG(LogTemp, Log, TEXT("GuardCounter ApplyDamageFromTrace"));

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
		return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC)
		
		return;
	UBattleComponent* BattleComponent = Character->GetBattleComponent();
	const TArray<FHitResult>& HitResults = BattleComponent->GetHitResult();

	TSet<TWeakObjectPtr<AActor>> DamagedActors;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
			continue;

		if (DamagedActors.Contains(HitActor))
			continue;

		// Target의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (!TargetASC)
			continue;
		
		DamagedActors.Add(HitActor);
		
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(GuardCounterDamageEffect, 1.f);

		FGameplayEffectContextHandle Context =
			SourceASC->MakeEffectContext();

		Context.AddSourceObject(this);

		Context.AddHitResult(Hit);

		SpecHandle.Data->SetSetByCallerMagnitude(
			TAG_Data_DamageMultiplier,
			DamageMultiplier
		);
		
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		AddAttackTypeToEffectSpec(*Spec);

		if (SpecHandle.IsValid() && TargetASC)
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(
				*SpecHandle.Data.Get(),
				TargetASC
			);

			ApplyHeatGE(1,SourceASC);
		}
	}

	Character->PlayGuardCounterFlash();
	BattleComponent->ClearHitResult();
}

void USK_GA_GuardCounter::OnStopAttackTrace_Server()
{
	Super::OnStopAttackTrace_Server();
}

void USK_GA_GuardCounter::OnMontageFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[GuardCounter] Montage Finished -> EndAbility"));

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true, // bReplicateEndAbility
		true // bWasCancelled
	);
}
