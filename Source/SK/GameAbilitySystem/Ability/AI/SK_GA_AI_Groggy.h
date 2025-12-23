#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_Groggy.generated.h"

UCLASS()
class SK_API USK_GA_AI_Groggy : public USK_GA_AI_Base
{
	GENERATED_BODY()

public:
	USK_GA_AI_Groggy();

	void Groggy(TObjectPtr<UAnimMontage> AnimMontage);

	UFUNCTION()
	void OnGroggyCompleted();
	
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
