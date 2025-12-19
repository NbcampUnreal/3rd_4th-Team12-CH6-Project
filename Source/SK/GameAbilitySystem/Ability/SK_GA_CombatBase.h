#pragma once

#include "CoreMinimal.h"
#include "SKGameplayAbility.h"
#include "SK_GA_CombatBase.generated.h"

UCLASS()
class SK_API USK_GA_CombatBase : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
};
