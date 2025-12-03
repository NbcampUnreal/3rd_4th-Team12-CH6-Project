#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_SimpleInteract.generated.h"

class ASKInteractableBase;

struct FSKInteractionData;

UCLASS()
class SK_API USK_GA_SimpleInteract : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_SimpleInteract();

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
	void OnMoveCompleted();
	
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnCanceled();

private:
	
	UPROPERTY()
	TObjectPtr<ASKInteractableBase> CachedTargetActor;
};
