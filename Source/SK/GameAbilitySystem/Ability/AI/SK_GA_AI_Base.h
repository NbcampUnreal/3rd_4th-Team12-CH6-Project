#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_AI_Base.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitDelay;
class UAbilityTask_ApplyRootMotionMoveToForce;
class UAbilityTask_ApplyRootMotionJumpForce;
class UAbilityTask_ApplyRootMotionMoveToActorForce;

UCLASS()
class SK_API USK_GA_AI_Base : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	FGameplayAbilitySpecHandle CachedHandle;
	const FGameplayAbilityActorInfo* CachedActorInfo;
	FGameplayAbilityActivationInfo CachedActivationInfo;
	UPROPERTY()
	ACharacter* CachedCharacter;
	UPROPERTY()
	AController* CachedController;

	UPROPERTY()
	TMap<FName, TObjectPtr<UAnimMontage>> Montages;
	UPROPERTY()
	TObjectPtr<UAnimMontage> AnimMontage;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* OwnEventTask1;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* OwnEventTask2;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CommonEventTask;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* OwnMontageTask;
	UPROPERTY()
	UAbilityTask_WaitDelay* OwnDelayTask;
	UPROPERTY()
	UAbilityTask_ApplyRootMotionMoveToForce* OwnRushTask;
	UPROPERTY()
	UAbilityTask_ApplyRootMotionJumpForce* OwnJumpRushTask;
	UPROPERTY()
	UAbilityTask_ApplyRootMotionMoveToActorForce* OwnFlyRushTask;
	

public:
	USK_GA_AI_Base();

	TObjectPtr<UAnimMontage> GetAnimMontage(FName MontageName);

	void WaitEndAbility();

	UFUNCTION()
	void OnWaitEndAbilityCompleted(FGameplayEventData EventData);

	void SetFocus() const;

	void ClearFocus() const;

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
};
