#pragma once

#include "CoreMinimal.h"
#include "SK_GA_AI_Base.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_AI_Melee.generated.h"

UCLASS()
class SK_API USK_GA_AI_Melee : public USK_GA_AI_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DamageEffectClasses;

private:
	UPROPERTY()
	TWeakObjectPtr<const AActor> HitActor;
	
public:
	USK_GA_AI_Melee();

	void ApplyDamageToTarget(TWeakObjectPtr<const AActor> TargetActor);

	void Melee(UAnimMontage* AnimMontage);

	UFUNCTION()
	void OnHitCompleted(FGameplayEventData EventData);
	
	UFUNCTION()
	void OnMeleeCompleted();

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
