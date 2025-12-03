#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Dodge.generated.h"

class USKCombatComponent;

UCLASS()
class SK_API USK_GA_Dodge : public USKGameplayAbility
{
	GENERATED_BODY()
public:
	USK_GA_Dodge();

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

	UFUNCTION()
	virtual void SetAnimMontage(USKCombatComponent* CombatComponent);

	UPROPERTY()
	UAnimMontage* DodgeMontage;
};
