// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USK_GA_Sprint : public USKGameplayAbility
{
	GENERATED_BODY()
public:

	USK_GA_Sprint();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Ability|Cost")
	TSubclassOf<UGameplayEffect> SprintStaminaCostEffect;
private:
	void ConsumeStamina();
	void EndSprint();
	FTimerHandle StaminaTimerHandle;
	
};
