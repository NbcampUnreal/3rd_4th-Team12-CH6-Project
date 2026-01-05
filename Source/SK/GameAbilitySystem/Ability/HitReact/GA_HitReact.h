// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "GA_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UGA_HitReact : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_HitReact();
	
protected:
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
		bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="HitReaction")
	TSubclassOf<UGameplayEffect> HitConditionEffect;

	/** 노말 히트 시 밀려나가는 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float PushStrength = 200.0f;
};
