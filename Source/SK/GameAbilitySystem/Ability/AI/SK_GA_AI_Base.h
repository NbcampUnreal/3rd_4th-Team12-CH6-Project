#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_AI_Base.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitDelay;
class UAbilityTask_ApplyRootMotionJumpForce;

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
	UAbilityTask_ApplyRootMotionJumpForce* OwnJumpTask;


public:
	USK_GA_AI_Base();

	TObjectPtr<UAnimMontage> GetAnimMontage(FName MontageName);

	void WaitEndAbility();

	UFUNCTION()
	void OnWaitEndAbilityCompleted(FGameplayEventData EventData);

	
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
