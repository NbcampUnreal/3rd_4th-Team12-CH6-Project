#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "SK_GA_AI_BaseCombat.generated.h"

class ASKBaseProjectile;

UCLASS()
class SK_API USK_GA_AI_BaseCombat : public USK_GA_AI_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> DamageEffectClasses;
	
	UPROPERTY()
	TWeakObjectPtr<const AActor> HitActor;

	FGameplayTag CurrentAttackType;

	float DamageMultiplier = 1.f;
	
	UPROPERTY()
	TMap<FName, TSubclassOf<ASKBaseProjectile>> ProjectileClasses;
	
	UPROPERTY()
	TSubclassOf<ASKBaseProjectile> ProjectileClass;

	
public:
	USK_GA_AI_BaseCombat();

	void AddDamageMultiplierByAttackType();
	
	void ApplyDamageToTarget(TWeakObjectPtr<const AActor> TargetActor);
	
	TObjectPtr<AActor> GetTargetActor() const;

	FVector GetPredictedTargetLocation(float PredictionTime) const;

	FVector GetPredictedToTargetDirection(const FVector& PredictedLocation) const;

	float GetRushTime(const UAnimMontage& LocalAnimMontage) const;

	TSubclassOf<ASKBaseProjectile> GetProjectileClass(FName ProjectileClassName);

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
