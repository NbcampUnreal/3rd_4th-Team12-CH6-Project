#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_Chase.generated.h"

UCLASS()
class SK_API USK_GA_AI_Chase : public USK_GA_AI_Base
{
	GENERATED_BODY()

public:
	USK_GA_AI_Chase();

	void Chase();
	
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
