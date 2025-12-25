// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/SK_GA_RightAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/SKPlayerCharacter.h"
#include "Component/BattleComponent.h"
#include "Component/SKCombatComponent.h"
#include "Controller/SKPlayerController.h"
#include "GameData/StaticData/ComboTableRow.h"
#include "GameFramework/Character.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKNativeGameplayTags.h"

USK_GA_RightAttack::USK_GA_RightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void USK_GA_RightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerState* SKPlayerState = Cast<ASKPlayerState>(GetOwningActorFromActorInfo());
	if (!IsValid(SKPlayerState))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
		
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
		
	CachedCharacter = Cast<ASKPlayerCharacter>(Character);
	UBattleComponent* CurrentBattleComponent = CachedCharacter->GetBattleComponent();

	CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_RightMelee, true);

	CurrentComboIndex = CheckCombo(ActorInfo);
	PrepareComboCache(CurrentBattleComponent->CurrentWeaponData);
	BindComboCache();

	FName SectionName = GetComboMontageSection(CurrentComboIndex);
	
	UE_LOG(LogTemp, Log,
		TEXT("[Combo] Index: %d | Section: %s"),
		CurrentComboIndex,
		*SectionName.ToString()
	);
	
	//몽타주 실행 추가
	UAnimMontage* Montage = CurrentBattleComponent->GetRightATKMontage(0);
	UAbilityTask_PlayMontageAndWait* PlayTask =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Montage,
		1.0f,
		SectionName
	);
	
	PlayTask->OnCompleted.AddDynamic(this, &USK_GA_RightAttack::OnMontageCompleted);
	PlayTask->OnInterrupted.AddDynamic(this, &USK_GA_RightAttack::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &USK_GA_RightAttack::OnMontageInterrupted);
	
	//사용 O
	CachedCharacter->SetLooseTag(TAG_State_Action_ATK, true);
	PlayTask->ReadyForActivation();
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
    	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void USK_GA_RightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

void USK_GA_RightAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

bool USK_GA_RightAttack::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	bool result = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	
	if (!result)
	{
		CachedCharacter->UpdateMovementTag_ATK(TAG_State_Action_ATK_LeftMelee, false);
	}
	
	return result;
}

void USK_GA_RightAttack::BindComboCache()
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

		FRightComboKey Key;
		Key.FromState = Row->FromState;
		Key.InputTag  = Row->InputTag;

		if (ComboCache.Contains(Key))
		{
			
			continue;
		}

		ComboCache.Add(Key, Row);
	}
}

void USK_GA_RightAttack::PrepareComboCache(USKWeaponData* WeaponData)
{
	if (!WeaponData)
		return;

	if (WeaponData->ComboTable)
	{
		CurrentComboTable = WeaponData->ComboTable;
	}
}

int32 USK_GA_RightAttack::CheckCombo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return 0;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (ASC->HasMatchingGameplayTag(TAG_Combo_Left4))
	{
		return 7;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_LLR))
	{
		return 6;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Left3))
	{
		return 5;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_LR))
	{
		return 4;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Left2))
	{
		return 3;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Right3))
	{
		return 2;
	}
	else if (ASC->HasMatchingGameplayTag(TAG_Combo_Right2))
	{
		return 1;
	}

	return 0;
}

FName USK_GA_RightAttack::GetComboMontageSection(int32 ComboIndex) const
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
		return FName(TEXT("Combo_LR"));
		
	case 4:
		return FName(TEXT("Combo_LRR"));

	case 5:
		return FName(TEXT("Combo_LLR"));

	case 6:
		return FName(TEXT("Combo_LLRR"));

	case 7:
		return FName(TEXT("Combo_LLLR"));
		
	default:
		return NAME_None;
	}
}

FGameplayTag USK_GA_RightAttack::GetComboTag(int32 ComboIndex) const
{
	switch (ComboIndex)
	{
	case 0:
		return TAG_Combo_Right1;
	case 1:
		return TAG_Combo_Right2;
	case 2:
		return TAG_Combo_Right3;
	case 3:
		return TAG_Combo_LR;
	case 4:
		return TAG_Combo_LRR;
	case 5:
		return TAG_Combo_LLR;
	case 6:
		return TAG_Combo_LLRR;
	case 7:
		return TAG_Combo_LLLR;
	default:
		break;
	}

	return FGameplayTag();
}

FGameplayTag USK_GA_RightAttack::GetComboGiveTag(int32 ComboIndex) const
{
	switch (ComboIndex)
	{
	case 0:
		return TAG_Combo_Right1;

	case 1:
		return TAG_Combo_Right2;
		
	case 2:
		return TAG_Combo_Right3;
		
	case 3:
		return TAG_Combo_LR;
	
	case 5:
		return TAG_Combo_LLR;
			
	default:
		return FGameplayTag();
	}
}

void USK_GA_RightAttack::ApplyComboStateEffect(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !CurrentComboTable)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	FGameplayTag ComboTag = GetComboGiveTag(CurrentComboIndex);

	// 이번 입력 (Right GA니까 고정)
	const FGameplayTag InputTag = TAG_Input_TestRight;

	// 캐시에서 찾기
	const FRightComboKey Key{ ComboTag, InputTag };
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



void USK_GA_RightAttack::OnMontageCompleted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void USK_GA_RightAttack::OnMontageInterrupted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		true
	);
}

void USK_GA_RightAttack::ApplyDamageFromTrace()
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
	
	TSet<TWeakObjectPtr<AActor>> DamagedActors;
	
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
			continue;
		
		if (DamagedActors.Contains(HitActor))
			continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (!TargetASC)
			continue;

		DamagedActors.Add(HitActor); 

		TSubclassOf<UGameplayEffect> EffectClass =
			LeftAttackDamageGE[CurrentComboIndex];


		FGameplayEffectContextHandle Context =
			SourceASC->MakeEffectContext();

		Context.AddSourceObject(this);

		Context.AddHitResult(Hit);

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(EffectClass, 1.f, Context);

		const FRightComboKey Key{GetComboTag(CurrentComboIndex), TAG_Input_TestRight};
		const FComboTableRow* Row = ComboCache.FindRef(Key);
		if (Row->ComboAttackType != FGameplayTag::EmptyTag)
		{
			AttackTypeTag = Row->ComboAttackType;
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
			AddAttackTypeToEffectSpec(*Spec);
		}
		
		if (SpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(
				*SpecHandle.Data.Get(),
				TargetASC
			);

			ApplyHeatGE(CurrentComboIndex,SourceASC);
		}
	}
}

void USK_GA_RightAttack::OnStopAttackTrace_Server()
{
	Super::OnStopAttackTrace_Server();
}
