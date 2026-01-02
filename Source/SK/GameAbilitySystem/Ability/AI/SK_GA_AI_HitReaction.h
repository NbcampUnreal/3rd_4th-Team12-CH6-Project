#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "Curves/CurveFloat.h"
#include "SK_GA_AI_HitReaction.generated.h"

UCLASS()
class SK_API USK_GA_AI_HitReaction : public USK_GA_AI_Base
{
	GENERATED_BODY()

public:
	USK_GA_AI_HitReaction();

	void HitReaction(TObjectPtr<UAnimMontage> LocalAnimMontage);

	UFUNCTION()
	void OnHitReactionCompleted();
	
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


	// ===============================
	// Smooth Knockback (Curve-less)
	// ===============================
	void StartSmoothKnockback(const FVector& InDirection);
	void TickSmoothKnockback();
	void StopSmoothKnockback();

	FVector KnockbackDirection;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackDistance = 25.f;   // 총 이동 거리
	UPROPERTY(EditDefaultsOnly)
	float KnockbackDuration = 0.15f;  // 이동 시간

	float ElapsedTime = 0.f;
	float PrevAlpha = 0.f;

	FTimerHandle KnockbackTimerHandle;

	FVector CalculateHitDirection(const FGameplayEventData* TriggerEventData) const;
};
