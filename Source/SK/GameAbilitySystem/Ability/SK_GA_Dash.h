// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Dash.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USK_GA_Dash : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_Dash();

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



protected:
	// 대시 설정값들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Dash")
	float DashDistance = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Dash")
	float DashDuration = 0.2f;

	
	FTimerHandle DashTimerHandle;

	// 대시 중일 때 붙일 태그
	UPROPERTY(EditDefaultsOnly, Category = "SK|Tags")
	FGameplayTag DashActiveTag;
};
