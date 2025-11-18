#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Interact.generated.h"

UCLASS()
class SK_API USK_GA_Interact : public USKGameplayAbility
{
	GENERATED_BODY()
public:
	USK_GA_Interact();

	// Tick에서 활성화
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	void LineTraceWithChannel();
	
	void TryInteract();
	
	UPROPERTY()
	FTimerHandle TraceTimerHandle;
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentHitActor;
};
