// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Guard.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Utility/SKNativeGameplayTags.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"

USK_GA_Guard::USK_GA_Guard()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Ability 식별용 (선택)
	AbilityTags.AddTag(TAG_Ability_Guard);

	ActivationOwnedTags.AddTag(TAG_State_Action_Guard);

	// 가드 중에는 공격 Ability 차단 (선택)
	//BlockAbilitiesWithTag.AddTag(어떤 태그들을 차단해야하나);
}

void USK_GA_Guard::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 스테미너 조건 체크
	if (!CanStartGuard(ASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		UE_LOG(LogTemp, Warning, TEXT("1111"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	USKActionComponent* ActionComponent = Char->GetActionComponent();
	if (!ActionComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("2222"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USKWeaponAnimData* WeaponAnimData = ActionComponent->GetWeaponAnimData();
	if (!WeaponAnimData) 
	{
		UE_LOG(LogTemp, Warning, TEXT("3333"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UAnimMontage* GuardMontage = WeaponAnimData->GuardMontage;
	if (!GuardMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("4444"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		GuardMontage,
		1.f
	);

	if (MontageTask)
	{
		MontageTask->OnInterrupted.AddDynamic(this, &USK_GA_Guard::K2_EndAbility);
		MontageTask->OnCancelled.AddDynamic(this, &USK_GA_Guard::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}
	else
	{
		return;
	}
	
	// 퍼펙트 가드 윈도우 시작
	StartPerfectGuardWindow();
}

void USK_GA_Guard::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(TAG_State_Action_Guard_Perfect);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PerfectGuardTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USK_GA_Guard::CanStartGuard(const UAbilitySystemComponent* ASC) const
{
	const USKAttributeSet* AttrSet = ASC->GetSet<USKAttributeSet>();
	if (!AttrSet)
	{
		return false;
	}

	return AttrSet->GetStamina() >= MinGuardStamina;
}

void USK_GA_Guard::StartPerfectGuardWindow()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	ASC->AddLooseGameplayTag(TAG_State_Action_Guard_Perfect);

	UWorld* World = GetWorld();
	if (!World) return;
	
	World->GetTimerManager().SetTimer(
		PerfectGuardTimerHandle,
		this,
		&USK_GA_Guard::EndPerfectGuardWindow,
		PerfectGuardDuration,
		false
	);
}

void USK_GA_Guard::EndPerfectGuardWindow()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(PerfectGuardTimerHandle);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(TAG_State_Action_Guard_Perfect);
	}
}