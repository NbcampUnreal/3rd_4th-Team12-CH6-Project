#pragma once

#include "CoreMinimal.h"
#include "SK_GA_AI_Base.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_AI_Die.generated.h"

UCLASS()
class SK_API USK_GA_AI_Die : public USK_GA_AI_Base
{
	GENERATED_BODY()
	
public:
	USK_GA_AI_Die();

	void WaitEvent();

	UFUNCTION()
	void OnWaitEventCompleted(FGameplayEventData EventData);
	
	void Die(UAnimMontage* AnimMontage);

	UFUNCTION()
	void OnDieCompleted();

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
