// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_Skill_01.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/BattleComponent.h"
#include "GameData/StaticData/ComboTableRow.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/SKNativeGameplayTags.h"

void USK_GA_Skill_01::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
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

	CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_Skill, true);

	CurrentComboIndex = CheckCombo(ActorInfo);
	PrepareComboCache(CurrentBattleComponent->CurrentWeaponData);
	BindComboCache();

	FName SectionName = GetComboMontageSection(CurrentComboIndex);

	//몽타주 실행 추가

	UAnimMontage* Montage = CurrentBattleComponent->GetSkillMontage(MontageIndex);

	UAbilityTask_PlayMontageAndWait* PlayTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Montage,
			1.0f,
			SectionName
		);

	PlayTask->OnCompleted.AddDynamic(this, &USK_GA_Skill_01::OnMontageCompleted);
	PlayTask->OnInterrupted.AddDynamic(this, &USK_GA_Skill_01::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &USK_GA_Skill_01::OnMontageInterrupted);

	//사용 O
	CachedCharacter->SetLooseTag(TAG_State_Action_ATK, true);
	PlayTask->ReadyForActivation();

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CurrentBattleComponent->NotifyUseSkill(MontageIndex, false);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	CurrentBattleComponent->NotifyUseSkill(MontageIndex, true);
}

void USK_GA_Skill_01::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                 bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
		return;

	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(Character);
	
	PlayerCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_Skill, false);
	PlayerCharacter->SetLooseTag(TAG_State_Action_ATK, false);


	//방어코드 추가
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
		return;
	MoveComp->SetMovementMode(MOVE_Walking);


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USK_GA_Skill_01::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

bool USK_GA_Skill_01::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                FGameplayTagContainer* OptionalRelevantTags) const
{
	bool result = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	
	// if (!result && CachedCharacter)
	// {
	// 	CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_Skill, false);
	// }

	return result;
}

void USK_GA_Skill_01::BindComboCache()
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

		FSkillComboKey Key;
		Key.FromState = Row->FromState;
		Key.InputTag = Row->InputTag;

		if (ComboCache.Contains(Key))
		{
			continue;
		}

		ComboCache.Add(Key, Row);
	}
}

void USK_GA_Skill_01::PrepareComboCache(USKWeaponData* WeaponData)
{
	if (!WeaponData)
		return;

	if (WeaponData->ComboTable)
	{
		CurrentComboTable = WeaponData->ComboTable;
	}
}

int32 USK_GA_Skill_01::CheckCombo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return 0;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (ASC->HasMatchingGameplayTag(TAG_Combo_Skill2))
	{
		return 2;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Skill3))
	{
		return 1;
	}

	return 0;
}

FName USK_GA_Skill_01::GetComboMontageSection(int32 ComboIndex) const
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

void USK_GA_Skill_01::OnMontageCompleted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void USK_GA_Skill_01::OnMontageInterrupted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		true
	);
}

void USK_GA_Skill_01::ApplyDamageFromTrace()
{
	Super::ApplyDamageFromTrace();

	//인덱스 판단 태그로 변경
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PC)
		return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC)
		return;

	UBattleComponent* BattleComponent = PC->GetBattleComponent();
	const TArray<FHitResult>& HitResults = BattleComponent->GetHitResult();

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
			continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (!TargetASC)
			continue;

		TSubclassOf<UGameplayEffect> EffectClass =
			SkillDamageGE[CurrentComboIndex];


		FGameplayEffectContextHandle Context =
			SourceASC->MakeEffectContext();

		Context.AddSourceObject(this);

		Context.AddHitResult(Hit);

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(EffectClass, 1.f, Context);

		if (SpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(
				*SpecHandle.Data.Get(),
				TargetASC
			);
		}
	}
}

void USK_GA_Skill_01::OnStopAttackTrace_Server()
{
	Super::OnStopAttackTrace_Server();
}
