#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Death.generated.h"

UCLASS()
class SK_API USK_GA_Death : public USKGameplayAbility
{
	GENERATED_BODY()

		
public:
	USK_GA_Death();

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
	void ShowDeathUI();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK")
	float RespawnDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK")
	float DeathUIDelay;
	
};
