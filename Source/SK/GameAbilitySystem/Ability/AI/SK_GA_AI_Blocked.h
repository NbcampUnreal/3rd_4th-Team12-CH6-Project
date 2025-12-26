#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_Blocked.generated.h"

UCLASS()
class SK_API USK_GA_AI_Blocked : public USK_GA_AI_Base
{
	GENERATED_BODY()
	
public:
	USK_GA_AI_Blocked();

	void Blocked(TObjectPtr<UAnimMontage> LocalAnimMontage);

	UFUNCTION()
	void OnBlockedCompleted();
	
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
