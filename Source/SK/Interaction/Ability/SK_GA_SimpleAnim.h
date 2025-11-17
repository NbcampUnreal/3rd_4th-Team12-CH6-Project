#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_SimpleAnim.generated.h"

UCLASS()
class SK_API USK_GA_SimpleAnim : public UGameplayAbility
{
	GENERATED_BODY()
public:
	USK_GA_SimpleAnim();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnCanceled();
};