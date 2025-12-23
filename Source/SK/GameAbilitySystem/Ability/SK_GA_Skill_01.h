// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SK_GA_CombatBase.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Skill_01.generated.h"

class ASKPlayerCharacter;
class USKWeaponData;
struct FComboTableRow;

USTRUCT()
struct FSkillComboKey
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag FromState;

	UPROPERTY()
	FGameplayTag InputTag;

	bool operator==(const FSkillComboKey& Other) const
	{
		return FromState == Other.FromState &&
			   InputTag  == Other.InputTag;
	}
};

FORCEINLINE uint32 GetTypeHash(const FSkillComboKey& Key)
{
	return HashCombine(
		GetTypeHash(Key.FromState),
		GetTypeHash(Key.InputTag)
	);
}


/**
 * 
 */
UCLASS()
class SK_API USK_GA_Skill_01 : public USK_GA_CombatBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle,
	                           const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility)
	override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const
	override;


	void BindComboCache();
	void PrepareComboCache(USKWeaponData* WeaponData);
	
	int32 CheckCombo(const FGameplayAbilityActorInfo* ActorInfo) const;
	FName GetComboMontageSection(int32 ComboIndex) const;
	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();
	
	virtual void ApplyDamageFromTrace() override;
	virtual void OnStopAttackTrace_Server() override;

	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	int32 MontageIndex;

	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> SkillDamageGE;

	UPROPERTY()
	TObjectPtr<ASKPlayerCharacter> CachedCharacter;

	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TSubclassOf<UGameplayEffect> TagGiveGE;

	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TObjectPtr<UDataTable> CurrentComboTable;

	TMap<FSkillComboKey, const FComboTableRow*> ComboCache;
	
	UPROPERTY()
	int32 CurrentComboIndex = 0;
};
