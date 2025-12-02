#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_Wander.generated.h"

UCLASS()
class SK_API USK_GA_AI_Wander : public USK_GA_AI_Base
{
	GENERATED_BODY()

private:
	float WanderRadius = 1000.0f;
	float DelayTime = 3.0f;

public:
	USK_GA_AI_Wander();
	
	void Wander();

	void WaitMoveComplete();
	
	UFUNCTION()
	void OnWaitMoveCompleteCompleted(FGameplayEventData EventData);
	
	void Delay(float DelayDuration);

	UFUNCTION()
	void OnDelayCompleted();

	
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
