#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_BaseCombat.h"
#include "SK_GA_AI_Backstep.generated.h"

UCLASS()
class SK_API USK_GA_AI_Backstep : public USK_GA_AI_BaseCombat
{
	GENERATED_BODY()

public:
	USK_GA_AI_Backstep();

	void Backstep();

	void WaitMoveComplete();
	
	UFUNCTION()
	void OnWaitMoveCompleteCompleted(FGameplayEventData EventData);

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
