#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SK_GA_Melee.generated.h"

UCLASS()
class SK_API USK_GA_Melee : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DamageEffectClasses;

	FGameplayAbilitySpecHandle CachedHandle;
	const FGameplayAbilityActorInfo* CachedActorInfo;
	FGameplayAbilityActivationInfo CachedActivationInfo;
	UPROPERTY()
	AController* CachedController;

private:
	UPROPERTY()
	TWeakObjectPtr<const AActor> HitActor;
	
public:
	USK_GA_Melee();

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
