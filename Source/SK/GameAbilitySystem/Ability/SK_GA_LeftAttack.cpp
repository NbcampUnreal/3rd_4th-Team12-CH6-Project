// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_LeftAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/BattleComponent.h"
#include "Component/SKCombatComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameData/StaticData/ComboTableRow.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_LeftAttack::USK_GA_LeftAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void USK_GA_LeftAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
		
	CachedCharacter = Cast<ASKPlayerCharacter>(Character);
	UBattleComponent* CurrentBattleComponent = CachedCharacter->GetBattleComponent();

	CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, true);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CurrentComboIndex = CheckCombo(ActorInfo);
	PrepareComboCache(CurrentBattleComponent->CurrentWeaponData);
	BindComboCache();

	FName SectionName = GetComboMontageSection(CurrentComboIndex);

	//몽타주 실행 추가
	UAnimMontage* Montage = CurrentBattleComponent->GetLeftATKMontage(0);

	UAbilityTask_PlayMontageAndWait* PlayTask =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Montage,
		1.0f,
		SectionName
	);
	
	PlayTask->OnCompleted.AddDynamic(this, &USK_GA_LeftAttack::OnMontageCompleted);
	PlayTask->OnInterrupted.AddDynamic(this, &USK_GA_LeftAttack::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &USK_GA_LeftAttack::OnMontageInterrupted);
	
	//사용 O
	CachedCharacter->SetLooseTag(TAG_State_Action_ATK, true);
	PlayTask->ReadyForActivation();
}

void USK_GA_LeftAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASKPlayerController* TempController = Cast<ASKPlayerController>(ActorInfo->PlayerController);
	if (bWasCancelled)
	{
		if (TempController)
		{
			if (TempController->GetCanMaintainCombo())
			{
				ApplyComboStateEffect(ActorInfo);
			}
			else
			{
				TempController->SetCanMaintainCombo(false);
			}
		}
	}
	else
	{
		ApplyComboStateEffect(ActorInfo);
		if (TempController)
		{
			TempController->SetCanMaintainCombo(false);
		}
	}

	CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	CachedCharacter->SetLooseTag(TAG_State_Action_ATK, false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USK_GA_LeftAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

bool USK_GA_LeftAttack::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	bool result = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	
	if (!result)
	{
		CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	}

	return result;
}

void USK_GA_LeftAttack::BindComboCache()
{
	if (!CurrentComboTable)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("BindComboCache failed: ComboTable is null"));
		return;
	}

	if (CurrentComboTable->GetRowStruct() != FComboTableRow::StaticStruct())
	{
		UE_LOG(LogTemp, Error,
			TEXT("BindComboCache failed: RowStruct mismatch"));
		return;
	}

	ComboCache.Empty();

	for (const auto& Pair : CurrentComboTable->GetRowMap())
	{
		const FComboTableRow* Row =
			reinterpret_cast<const FComboTableRow*>(Pair.Value);

		if (!Row)
		{
			continue;
		}

		FLeftComboKey Key;
		Key.FromState = Row->FromState;
		Key.InputTag  = Row->InputTag;

		if (ComboCache.Contains(Key))
		{
			
			continue;
		}

		ComboCache.Add(Key, Row);
	}
}

void USK_GA_LeftAttack::PrepareComboCache(USKWeaponData* WeaponData)
{
	if (!WeaponData)
		return;

	if (WeaponData->ComboTable)
	{
		CurrentComboTable = WeaponData->ComboTable;
	}
}

int32 USK_GA_LeftAttack::CheckCombo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return 0;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (ASC->HasMatchingGameplayTag(TAG_Combo_Left4))
	{
		return 3;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Left3))
	{
		return 2;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Left2))
	{
		return 1;
	}

	return 0;
}

FName USK_GA_LeftAttack::GetComboMontageSection(int32 ComboIndex) const
{
	switch (ComboIndex)
	{
	case 0:
		return FName(TEXT("Combo_01"));

	case 1:
		return FName(TEXT("Combo_02"));

	case 2:
		return FName(TEXT("Combo_03"));

	case 3:
		return FName(TEXT("Combo_04"));

	default:
		return NAME_None;
	}
}

void USK_GA_LeftAttack::ApplyComboStateEffect(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !CurrentComboTable)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	FGameplayTag ComboTag;

	if (CurrentComboIndex == 3)
	{
		ComboTag = TAG_Combo_Left4;
	}
	else if (CurrentComboIndex == 2)
	{
		ComboTag = TAG_Combo_Left3;
	}
	else if (CurrentComboIndex == 1)
	{
		ComboTag = TAG_Combo_Left2;
	}
	else if (CurrentComboIndex == 0)
	{
		ComboTag = TAG_Combo_Left1;
	}

	// 이번 입력 (LeftAttack GA니까 고정)
	const FGameplayTag InputTag = TAG_Input_TestLeft;

	// 캐시에서 찾기
	const FLeftComboKey Key{ ComboTag, InputTag };
	const FComboTableRow* Row = ComboCache.FindRef(Key);

	FGameplayTag GiveTag;
	
	if (!Row)
	{
		if (CurrentComboIndex == 0)
		{
			GiveTag = TAG_Combo_Left1;
		}
		else
		{
			GiveTag = FGameplayTag::EmptyTag;
		}
	}
	else if (Row->ToState.IsValid())
	{
		GiveTag = Row->ToState;
	}
	
	TSubclassOf<UGameplayEffect> EffectClass = TagGiveGE; // 태그용 GE
	if (!EffectClass)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);

	if (!SpecHandle.IsValid())
	{
		return;
	}
	
	SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(GiveTag);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void USK_GA_LeftAttack::OnMontageCompleted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void USK_GA_LeftAttack::OnMontageInterrupted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		true
	);
}

void USK_GA_LeftAttack::ApplyDamageFromTrace()
{
	//인덱스 판단 태그로 변경
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;
	USKCombatComponent* CombatComponent = PC->GetCombatComponent();
	
	for (AActor* HitActor : CombatComponent->GetHitActors())
	{
		if (!HitActor)
			continue;

		TSubclassOf<UGameplayEffect> EffectClass = LeftAttackDamageGE[CurrentComboIndex];

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(EffectClass, 1.f);

		// Target의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (SpecHandle.IsValid() && TargetASC)
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void USK_GA_LeftAttack::OnStopAttackTrace_Server()
{
	//몽타주 1번에 여러 공격이 들어갈 때 데미지 빨리 처리 시 불리는 함수
	ApplyDamageFromTrace();
}
