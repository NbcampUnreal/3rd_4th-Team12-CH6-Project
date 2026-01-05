#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_BaseCombat.h"
#include "SK_GA_AI_ProjectileAttack.generated.h"

class ASKBaseProjectile;

UCLASS()
class SK_API USK_GA_AI_ProjectileAttack : public USK_GA_AI_BaseCombat
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<ASKBaseProjectile> Projectile;

	
public:
	USK_GA_AI_ProjectileAttack();

	void WaitAnimNotify();

	void WaitHit();
	
	void SpawnProjectile();

	void LaunchProjectile(TObjectPtr<UAnimMontage> LocalAnimMontage);

	UFUNCTION()
	void OnWaitAnimNotifyCompleted(FGameplayEventData EventData);

	UFUNCTION()
	void OnWaitHitCompleted(FGameplayEventData EventData);
	
	UFUNCTION()
	void OnLaunchProjectileCompleted();
	
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
