// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Guard.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Utility/SKNativeGameplayTags.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "Weapon/ActionData/SKWeaponAnimData.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "Character/SKPlayerCharacter.h"

USK_GA_Guard::USK_GA_Guard()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Ability 식별용 (선택)
	SetAssetTags(FGameplayTagContainer(TAG_Ability_Guard));

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

	// 가드 발동하기 위한 스테미너 조건 체크
	if (!CanStartGuard(ASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
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
	
	GuardMontage = WeaponAnimData->GuardMontage;
	if (!GuardMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	GuardMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		GuardMontage,
		1.f
	);

	if (GuardMontageTask)
	{
		GuardMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_Guard::K2_EndAbility);
		GuardMontageTask->OnCancelled.AddDynamic(this, &USK_GA_Guard::K2_EndAbility);
		GuardMontageTask->ReadyForActivation();
	}
	else
	{
		return;
	}
	
	// Guard 성공 이벤트 대기
	GuardSuccessEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			TAG_Event_Guard_Success,
			nullptr,
			false,
			false
		);

	if (GuardSuccessEventTask)
	{
		GuardSuccessEventTask->EventReceived.AddDynamic(
			this,
			&USK_GA_Guard::OnGuardSuccess
		);
		GuardSuccessEventTask->ReadyForActivation();
	}

	//가드유지를 위한 스테미너 체크
	WaitStaminaChangeTask =
	UAbilityTask_WaitAttributeChange::WaitForAttributeChange(
		this,
		USKAttributeSet::GetStaminaAttribute(),
		FGameplayTag(),
		FGameplayTag(),
		false,
		nullptr
	);

	if (WaitStaminaChangeTask)
	{
		WaitStaminaChangeTask->OnChange.AddDynamic(
			this,
			&USK_GA_Guard::OnStaminaChanged
		);
		WaitStaminaChangeTask->ReadyForActivation();
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

void USK_GA_Guard::OnGuardSuccess(FGameplayEventData Payload)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		ASC->ExecuteGameplayCue(TAG_GameplayCue_Guard_Block);
	}

	ASC->AddLooseGameplayTag(TAG_State_Action_Guard_Success);

	// 짧은 시간만 유지 (예: 0.3초)
	FTimerHandle GuardSuccessTimer;
	GetWorld()->GetTimerManager().SetTimer(
		GuardSuccessTimer,
		[ASC]()
		{
			ASC->RemoveLooseGameplayTag(TAG_State_Action_Guard_Success);
		},
		1.5f,
		false
	);
}

void USK_GA_Guard::OnStaminaChanged()
{
	// 이미 종료된 상태면 무시
	if (!IsActive()) return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const float CurrentStamina =
		ASC->GetNumericAttribute(USKAttributeSet::GetStaminaAttribute());

	if (CurrentStamina <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stamina depleted -> Force Guard End"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
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