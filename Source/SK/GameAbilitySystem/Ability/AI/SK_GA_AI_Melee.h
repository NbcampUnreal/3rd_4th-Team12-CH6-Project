#pragma once

#include "CoreMinimal.h"
#include "SK_GA_AI_BaseCombat.h"
#include "SK_GA_AI_Melee.generated.h"

UCLASS()
class SK_API USK_GA_AI_Melee : public USK_GA_AI_BaseCombat
{
	GENERATED_BODY()
	
public:
	USK_GA_AI_Melee();
	
	void Melee(TObjectPtr<UAnimMontage> AnimMontage);

	UFUNCTION()
	void OnHitCompleted(FGameplayEventData EventData);
	
	UFUNCTION()
	void OnMeleeCompleted();

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
