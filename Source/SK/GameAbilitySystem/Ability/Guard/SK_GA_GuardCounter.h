// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_GuardCounter.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USK_GA_GuardCounter : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_GuardCounter();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	virtual void ApplyDamageFromTrace() override;
	virtual void OnStopAttackTrace_Server() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GuardCounter")
	TSubclassOf<UGameplayEffect> GuardCounterDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GuardCounter")
	float DamageMultiplier = 1.5f;

	UPROPERTY()
	TObjectPtr<UAnimMontage> GuardCounterMontage;
};
