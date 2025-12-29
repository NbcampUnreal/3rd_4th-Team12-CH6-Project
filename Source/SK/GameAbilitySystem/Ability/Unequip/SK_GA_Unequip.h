#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Unequip.generated.h"

UCLASS()
class SK_API USK_GA_Unequip : public USKGameplayAbility
{
	GENERATED_BODY()

public:

	USK_GA_Unequip();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnCanceled();
};
