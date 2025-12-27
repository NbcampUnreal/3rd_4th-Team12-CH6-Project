#pragma once

#include "CoreMinimal.h"
#include "Interaction/Ability/SK_GA_InteractionTemplate.h"
#include "SK_GA_Traversal_AvoidObstacle.generated.h"

UCLASS()
class SK_API USK_GA_Traversal_AvoidObstacle : public USK_GA_InteractionTemplate
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	void SetEndAbility();
	
	virtual void OnMoveCompleted() override;
};
