#pragma once

#include "CoreMinimal.h"
#include "SK_GA_InteractionTemplate.h"
#include "SK_GA_Interaction_Openable.generated.h"

UCLASS()
class SK_API USK_GA_Interaction_Openable : public USK_GA_InteractionTemplate
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void OnMoveCompleted() override;

};
