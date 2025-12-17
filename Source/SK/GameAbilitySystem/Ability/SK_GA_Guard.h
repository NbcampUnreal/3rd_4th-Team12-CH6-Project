// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Guard.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class SK_API USK_GA_Guard : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_Guard();
	
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
	/** 퍼펙트 가드 윈도우 시작 */
	void StartPerfectGuardWindow();

	/** 퍼펙트 가드 윈도우 종료 */
	void EndPerfectGuardWindow();

	/** 가드 시작 가능 여부 (스테미너 체크) */
	bool CanStartGuard(const UAbilitySystemComponent* ASC) const;

protected:
	/** 퍼펙트 가드 판정 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float PerfectGuardDuration = 0.5f;

	/** 가드 시작 최소 스테미너 */
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float MinGuardStamina = 10.f;

private:
	FTimerHandle PerfectGuardTimerHandle;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
