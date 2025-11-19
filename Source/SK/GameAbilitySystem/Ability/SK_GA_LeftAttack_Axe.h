// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_LeftAttack_Axe.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USK_GA_LeftAttack_Axe : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_LeftAttack_Axe();
	
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



	void ApplyDamageFromTrace();

	UPROPERTY(EditDefaultsOnly, Category = "Event")
	FGameplayTag Event_LeftATKTraceEnd;
	
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> LeftAttackDamageGE;

	
};
