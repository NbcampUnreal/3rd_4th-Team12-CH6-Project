#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SK_GA_InteractionBase.generated.h"

struct FSKInteractionData;
class ASKInteractableBase;

UCLASS()
class SK_API USK_GA_InteractionBase : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_InteractionBase();

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

	UPROPERTY()
	TObjectPtr<ASKInteractableBase> CachedTargetActor;
	
	UPROPERTY()
	FSKInteractionData CachedInteractionData;
};
