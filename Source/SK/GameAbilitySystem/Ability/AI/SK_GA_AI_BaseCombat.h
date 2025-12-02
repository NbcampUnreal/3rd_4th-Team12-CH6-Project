#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_BaseCombat.generated.h"

UCLASS()
class SK_API USK_GA_AI_BaseCombat : public USK_GA_AI_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DamageEffectClasses;
	
	UPROPERTY()
	TWeakObjectPtr<const AActor> HitActor;

public:
	USK_GA_AI_BaseCombat();

	void ApplyDamageToTarget(TWeakObjectPtr<const AActor> TargetActor);

	AActor* GetTargetActor() const;

	void SetFocus() const;

	void ClearFocus() const;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
};
