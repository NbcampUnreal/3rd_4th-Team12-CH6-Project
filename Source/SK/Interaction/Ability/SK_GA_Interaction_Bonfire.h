#pragma once

#include "CoreMinimal.h"
#include "SK_GA_InteractionTemplate.h"
#include "SK_GA_Interaction_Bonfire.generated.h"

UCLASS()
class SK_API USK_GA_Interaction_Bonfire : public USK_GA_InteractionTemplate
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void OnMoveCompleted() override;

	FVector GetTargetLocation(const ASKPlayerCharacter* PlayerCharacter) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float TargetDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float DistanceThreshold;
};
